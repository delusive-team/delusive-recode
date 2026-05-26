// Shared ESP – Client-side (header-only, NO winsock headers – dynamic loading)
// This avoids all conflicts with windows.h / winsock.h in the host DLL.
#pragma once
#define WIN32_LEAN_AND_MEAN
#define _WINSOCKAPI_   // Prevent inclusion of winsock.h in windows.h
#include <windows.h>
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>
#include <cstring>
#include <cstdio>
#include <cstdlib>

static constexpr char SH_FILL = '\xA6';
static constexpr int  SH_PKT  = 1024;

// ─── Minimal Winsock types (no winsock.h / winsock2.h needed) ────────────────
typedef UINT_PTR    SH_SOCK;
#define SH_BAD_SOCK ((SH_SOCK)(~0))

struct sh_in_addr  { unsigned long s_addr_val; };
struct sh_sockaddr_in {
    short         sin_family;
    unsigned short sin_port;
    sh_in_addr    sin_addr;
    char          sin_zero[8];
};
struct sh_sockaddr { short sa_family; char sa_data[14]; };
struct sh_addrinfo {
    int          ai_flags, ai_family, ai_socktype, ai_protocol;
    size_t       ai_addrlen;
    char*        ai_canonname;
    sh_sockaddr* ai_addr;
    sh_addrinfo* ai_next;
};
struct sh_fd_set  { unsigned int fd_count; SH_SOCK fd_array[64]; };
struct sh_timeval { long tv_sec; long tv_usec; };
struct sh_wsadata { WORD wVersion, wHighVersion; char szDescription[257], szSystemStatus[129]; unsigned short iMaxSockets, iMaxUdpDg; char* lpVendorInfo; };

// ─── Dynamic function pointers ────────────────────────────────────────────────
typedef int      (WINAPI* fn_WSAStartup)(WORD, sh_wsadata*);
typedef int      (WINAPI* fn_WSACleanup)();
typedef SH_SOCK  (WINAPI* fn_socket)(int, int, int);
typedef int      (WINAPI* fn_connect)(SH_SOCK, const sh_sockaddr*, int);
typedef int      (WINAPI* fn_send)(SH_SOCK, const char*, int, int);
typedef int      (WINAPI* fn_recv)(SH_SOCK, char*, int, int);
typedef int      (WINAPI* fn_select)(int, sh_fd_set*, sh_fd_set*, sh_fd_set*, const sh_timeval*);
typedef int      (WINAPI* fn_ioctlsocket)(SH_SOCK, long, unsigned long*);
typedef int      (WINAPI* fn_closesocket)(SH_SOCK);
typedef int      (WINAPI* fn_getaddrinfo)(const char*, const char*, const sh_addrinfo*, sh_addrinfo**);
typedef void     (WINAPI* fn_freeaddrinfo)(sh_addrinfo*);

static struct sh_ws2 {
    fn_WSAStartup   WSAStartup   = nullptr;
    fn_WSACleanup   WSACleanup   = nullptr;
    fn_socket       socket       = nullptr;
    fn_connect      connect      = nullptr;
    fn_send         send         = nullptr;
    fn_recv         recv         = nullptr;
    fn_select       select       = nullptr;
    fn_ioctlsocket  ioctlsocket  = nullptr;
    fn_closesocket  closesocket  = nullptr;
    fn_getaddrinfo  getaddrinfo  = nullptr;
    fn_freeaddrinfo freeaddrinfo = nullptr;
    bool loaded = false;

    bool load() {
        if (loaded) return true;
        HMODULE m = LoadLibraryA("ws2_32.dll");
        if (!m) return false;
#define LOAD(fn) fn = (fn_##fn)GetProcAddress(m, #fn); if (!fn) return false;
        LOAD(WSAStartup) LOAD(WSACleanup) LOAD(socket) LOAD(connect)
        LOAD(send) LOAD(recv) LOAD(select) LOAD(ioctlsocket)
        LOAD(closesocket) LOAD(getaddrinfo) LOAD(freeaddrinfo)
#undef LOAD
        return loaded = true;
    }
} g_ws2;

// ─── Entity data ──────────────────────────────────────────────────────────────
struct sh_entity_t {
    unsigned long userid = 0;
    float x = 0, y = 0, z = 0;
    int   hp = 100;
    char  name[33] = {};
    bool  is_peer = false;

    void to_pkt(char* pkt) const {
        memset(pkt, SH_FILL, SH_PKT);
        pkt[4] = is_peer ? 'P' : 'E';
        auto ws = [&](int off, const char* s, int maxlen) {
            int n = 0;
            while (s[n] && n < maxlen) n++; // safe strlen
            memcpy(pkt + off, s, n);
        };
        char buf[32];
        snprintf(buf, sizeof(buf), "%lu",  userid); ws(8,  buf, 20);
        snprintf(buf, sizeof(buf), "%.4f", x);      ws(32, buf, 15);
        snprintf(buf, sizeof(buf), "%.4f", y);      ws(48, buf, 15);
        snprintf(buf, sizeof(buf), "%.4f", z);      ws(64, buf, 15);
        snprintf(buf, sizeof(buf), "%d",   hp);     ws(80, buf,  8);
        ws(88, name, 32);
    }

    static sh_entity_t from_pkt(const char* pkt) {
        sh_entity_t e;
        e.is_peer = (pkt[4] == 'P');
        auto rs = [&](int off, int max, char* out, int outsz) {
            int n = 0;
            for (int i = off; i < off + max && n < outsz - 1; i++) {
                if ((unsigned char)pkt[i] == (unsigned char)SH_FILL) break;
                out[n++] = pkt[i];
            }
            out[n] = 0;
        };
        char buf[64] = {};
        rs(8,  20, buf, 64); if (buf[0]) e.userid = strtoul(buf, nullptr, 10);
        rs(32, 15, buf, 64); if (buf[0]) e.x  = (float)atof(buf);
        rs(48, 15, buf, 64); if (buf[0]) e.y  = (float)atof(buf);
        rs(64, 15, buf, 64); if (buf[0]) e.z  = (float)atof(buf);
        rs(80,  8, buf, 64); if (buf[0]) e.hp = atoi(buf);
        rs(88, 32, e.name, sizeof(e.name));
        return e;
    }
};

// ─── Shared ESP client ────────────────────────────────────────────────────────
class c_shared_esp {
public:
    std::mutex               ents_mtx;
    std::vector<sh_entity_t> ents_from_peers;

    std::atomic<bool> connected{ false };
    std::atomic<bool> enabled  { false };
    char status_text[128] = "Disconnected";
    char server_ip[64]    = "26.50.34.197";
    int  server_port      = 32947;

    void push_frame(const sh_entity_t& self, const std::vector<sh_entity_t>& enemies) {
        if (!connected.load()) return;
        std::lock_guard<std::mutex> lk(_frame_mtx);
        _self = self; _vis = enemies; _dirty = true;
    }

    void connect(const char* ip, int port) {
        disconnect();
        strncpy_s(server_ip, ip, _TRUNCATE);
        server_port = port;
        enabled.store(true);
        std::thread(&c_shared_esp::_worker, this).detach();
    }

    void disconnect() {
        enabled.store(false);
        _close_sock();
        connected.store(false);
        std::lock_guard<std::mutex> lk(ents_mtx);
        ents_from_peers.clear();
        snprintf(status_text, sizeof(status_text), "Disconnected");
    }

private:
    SH_SOCK    _sock = SH_BAD_SOCK;
    std::mutex _frame_mtx, _sock_mtx;
    sh_entity_t            _self{};
    std::vector<sh_entity_t> _vis;
    bool _dirty = false;

    void _close_sock() {
        std::lock_guard<std::mutex> lk(_sock_mtx);
        if (_sock != SH_BAD_SOCK) {
            if (g_ws2.loaded) g_ws2.closesocket(_sock);
            _sock = SH_BAD_SOCK;
        }
    }

    void _worker() {
        if (!g_ws2.load()) {
            snprintf(status_text, sizeof(status_text), "ws2_32.dll load failed");
            enabled.store(false); return;
        }
        sh_wsadata wd{};
        g_ws2.WSAStartup(0x0202, &wd);

        while (enabled.load()) {
            snprintf(status_text, sizeof(status_text), "Connecting to %s:%d...", server_ip, server_port);
            SH_SOCK s = _do_connect();
            if (s == SH_BAD_SOCK) {
                snprintf(status_text, sizeof(status_text), "Connection failed – retry in 3s");
                Sleep(3000); continue;
            }
            { std::lock_guard<std::mutex> lk(_sock_mtx); _sock = s; }

            // handshake
            char hello[1] = { 'M' };
            if (g_ws2.send(_sock, hello, 1, 0) != 1) { _reset(); continue; }
            char hs[SH_PKT] = {};
            if (_recv_all(hs, SH_PKT) != SH_PKT || hs[1] != 'H') {
                snprintf(status_text, sizeof(status_text), "Handshake failed");
                _reset(); continue;
            }
            connected.store(true);
            snprintf(status_text, sizeof(status_text), "Connected to %s:%d", server_ip, server_port);

            char pkt[SH_PKT] = {};
            while (enabled.load()) {
                Sleep(100);

                sh_entity_t local_self;
                std::vector<sh_entity_t> vis;
                { std::lock_guard<std::mutex> lk(_frame_mtx); local_self = _self; vis = _vis; _dirty = false; }

                // 1. Send own data
                local_self.to_pkt(pkt);
                if (_send_all(pkt, SH_PKT) != SH_PKT) break;

                // 2. Send enemy count
                memset(pkt, SH_FILL, SH_PKT);
                snprintf(pkt, 12, "%zu", vis.size());
                if (_send_all(pkt, SH_PKT) != SH_PKT) break;

                // 3. Send each enemy
                bool ok = true;
                for (size_t i = 0; i < vis.size(); i++) {
                    vis[i].to_pkt(pkt);
                    if (_send_all(pkt, SH_PKT) != SH_PKT) { ok = false; break; }
                }
                if (!ok) break;

                // 4. Receive merged count
                memset(pkt, 0, SH_PKT);
                if (_recv_all(pkt, SH_PKT) != SH_PKT) break;

                char cnt_str[16] = {}; int n = 0;
                for (int i = 0; i < 15; i++) {
                    if ((unsigned char)pkt[i] == (unsigned char)SH_FILL || pkt[i] == 0) break;
                    cnt_str[n++] = pkt[i];
                }
                int total = n > 0 ? atoi(cnt_str) : 0;

                // 5. Receive each entity
                std::vector<sh_entity_t> new_ents;
                new_ents.reserve(total);
                ok = true;
                for (int i = 0; i < total; i++) {
                    char ep[SH_PKT] = {};
                    if (_recv_all(ep, SH_PKT) != SH_PKT) { ok = false; break; }
                    new_ents.push_back(sh_entity_t::from_pkt(ep));
                }
                if (!ok) break;

                { std::lock_guard<std::mutex> lk(ents_mtx); ents_from_peers = std::move(new_ents); }
            }
            _reset();
        }
        g_ws2.WSACleanup();
    }

    SH_SOCK _do_connect() {
        sh_addrinfo hints = {}, *res = nullptr;
        hints.ai_family   = 2;  // AF_INET
        hints.ai_socktype = 1;  // SOCK_STREAM
        hints.ai_protocol = 6;  // IPPROTO_TCP
        char pstr[8]; snprintf(pstr, sizeof(pstr), "%d", server_port);
        if (g_ws2.getaddrinfo(server_ip, pstr, &hints, &res) != 0) return SH_BAD_SOCK;

        SH_SOCK s = g_ws2.socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (s == SH_BAD_SOCK) { g_ws2.freeaddrinfo(res); return SH_BAD_SOCK; }

        // Non-blocking connect with 3s timeout
        unsigned long nb = 1; g_ws2.ioctlsocket(s, 0x8004667EL, &nb);  // FIONBIO
        g_ws2.connect(s, res->ai_addr, (int)res->ai_addrlen);
        g_ws2.freeaddrinfo(res);

        sh_fd_set fds = {}; fds.fd_count = 1; fds.fd_array[0] = s;
        sh_timeval tv = { 3, 0 };
        if (g_ws2.select(0, nullptr, &fds, nullptr, &tv) <= 0) {
            g_ws2.closesocket(s); return SH_BAD_SOCK;
        }
        nb = 0; g_ws2.ioctlsocket(s, 0x8004667EL, &nb);
        return s;
    }

    void _reset() {
        connected.store(false);
        _close_sock();
        { std::lock_guard<std::mutex> lk(ents_mtx); ents_from_peers.clear(); }
    }

    int _send_all(const char* b, int len) {
        int s = 0;
        while (s < len) {
            int r = g_ws2.send(_sock, b + s, len - s, 0);
            if (r <= 0) return -1; s += r;
        }
        return s;
    }
    int _recv_all(char* b, int len) {
        int g = 0;
        while (g < len) {
            int r = g_ws2.recv(_sock, b + g, len - g, 0);
            if (r <= 0) return -1; g += r;
        }
        return g;
    }
};

inline c_shared_esp g_shared_esp;

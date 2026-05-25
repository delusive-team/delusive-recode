#include "il2cpp.h"
#include "../../vcruntime/logger/logger.h"
bool il2cpp::initialize()
{
    LOG("Initializing IL2CPP...");
    unity_player = pe::get_image(HASH("unityplayer"));
    game_assembly = pe::get_image(HASH("gameassembly"));

    if (!unity_player || !game_assembly) {
        LOG_ERROR("Failed to get unity_player (%#llx) or game_assembly (%#llx)", unity_player, game_assembly);
        return false;
    }

    assign_type(array_new);
    assign_type(string_new);
    assign_type(object_new);
    assign_type(domain_get);
    assign_type(resolve_icall);
    assign_type(thread_attach);
    assign_type(class_get_type);
    assign_type(type_get_object);
    assign_type(class_from_name);
    assign_type(class_get_fields);
    assign_type(field_get_offset);
    assign_type(class_get_methods);
    assign_type(string_new_wrapper);
    assign_type(assembly_get_image);
    assign_type(domain_get_assemblies);
    assign_type(method_get_param_name);
    assign_type(method_get_param_count);
    assign_type(method_get_return_type);
    assign_type(field_static_get_value);
    assign_type(field_static_set_value);
    assign_type(class_get_method_from_name);
    assign_type(class_get_static_field_data);
    assign_type(class_get_name);
    assign_type(class_get_nested_types);
    
    LOG_OK("IL2CPP types assigned successfully!");
    return true;
}

uintptr_t il2cpp::impl::_get_class(const char* ns, const char* klass_name)
{
    const auto& domain = il2cpp_call(domain_get)();

    std::size_t count = 0;
    const auto& assemblies = il2cpp_call(domain_get_assemblies)(domain, &count);

    for (size_t idx = 0; idx < count; idx++)
    {
        const auto& assembly = assemblies[idx];
        if (!memory::is_valid(assembly))
            continue;

        const auto& img = il2cpp_call(assembly_get_image)(assembly);
        if (!memory::is_valid(img))
            continue;

        const auto& klass = il2cpp_call(class_from_name)(img, ns, klass_name);
        if (!memory::is_valid(klass))
            continue;

        return klass;
    }

    il2cpp::resolve_errors++;
    LOG_ERROR("%s not found!", klass_name);
    PAUSE();
    return 0;
}

uintptr_t il2cpp::impl::_get_nested_class(uintptr_t klass, const char* name)
{
    if (!memory::is_valid(klass))
        return 0;

    uintptr_t iter = 0;
    while (auto nested = il2cpp_call(class_get_nested_types)(klass, &iter))
    {
        std::string current_name = il2cpp_call(class_get_name)(nested);
        if (current_name.empty())
            continue;

        if (current_name == name)
            return nested;
    }

    il2cpp::resolve_errors++;
    LOG_ERROR("%s nested class not found!", name);
    PAUSE();
    return 0;
}

uintptr_t il2cpp::impl::_get_method(uintptr_t klass, const char* name, int argument_number)
{
    const auto& method = il2cpp_call(class_get_method_from_name)(klass, name, argument_number);
    if (!memory::is_valid(method))
    {
        il2cpp::resolve_errors++;
        LOG_ERROR("%s not found!", name);
        PAUSE();
        return { };
    }

    return method;
}

uintptr_t il2cpp::impl::_get_field(uintptr_t klass, const char* name, bool offset)
{
    uintptr_t iterator = { };

    while (auto field = il2cpp_call(class_get_fields)(klass, &iterator))
    {
        std::string current_name = mem::read< const char* >(field);
        if (current_name.empty())
            continue;

        if (current_name != name && current_name != std::string("<") + name + ">k__BackingField")
            continue;

        if (!offset)
            return field;

        return il2cpp_call(field_get_offset)(field);
    }

    return { };
}

uintptr_t il2cpp::get_virtual_method(const char* ns, const char* klass_name, const char* method_name, int argument_number)
{
    const auto& klass = get_class(ns, klass_name);
    if (!memory::is_valid(klass))
        return { };

    const auto& method = mem::read< uintptr_t >(get_method(klass, method_name, argument_number));
    if (!memory::is_valid(method))
        return { };

    for (auto idx = klass; idx <= klass + 0x10000; idx += 0x1)
    {
        const auto& addr = mem::read< uintptr_t >(idx);
        if (addr == method)
        {
            return idx;
        }
    }

    il2cpp::resolve_errors++;
    LOG_ERROR("%s virtual not found!", method_name);
    PAUSE();
    return { };
}

uintptr_t il2cpp::new_string(const char* string)
{
    return il2cpp_call(string_new)(string);
}

uintptr_t il2cpp::new_array(uintptr_t array, int length)
{
    return il2cpp_call(array_new)(array, length);
}

uintptr_t il2cpp::type_object(const char* ns, const char* name)
{
    const auto& klass = get_class(ns, name);
    if (!memory::is_valid(klass))
        return { };

    const auto& class_type = il2cpp_call(class_get_type)(klass);
    if (!memory::is_valid(class_type))
        return { };

    return il2cpp_call(type_get_object)(class_type);
}
#include "tree.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "create_wrapper.h"
#include "globals.h"

void
WrapperCreator::create_wrapper(Namespace* ns)
{
    // hpp file
    hppout
        << "/**\n"
        << " * WARNING: This file is automatically generated from '"
        << inputfile << "' - do not change\n"
        << " */\n"
        << "#ifndef __" << modulename << "_WRAPPER_H__\n"
        << "#define __" << modulename << "_WRAPPER_H__\n"
        << "\n"
        << "#include \"wrapper_util.h\"\n"
        << "\n"
        << "extern WrappedFunction " << modulename << "_global_functions[];\n"
        << "extern WrappedClass " << modulename << "_classes[];\n"
        << "\n"
        << "#endif\n"
        << "\n";
    
    // cpp header
    out << "/**\n"
        << " * WARNING: This file is automatically generated from '"
        << inputfile << "' - do not change\n"
        << " */\n"
        << "\n"
        << "#include <new>\n"
        << "#include <string>\n"
        << "#include <squirrel.h>\n"
        << "#include \"wrapper_util.h\"\n"
        << "#include \"wrapper.interface.h\"\n"
        << "\n";
    if(selected_namespace != "") {
        out << "using namespace " << selected_namespace << ";\n";
        out << "\n";
    }
    
    for(std::vector<AtomicType*>::iterator i = ns->types.begin();
            i != ns->types.end(); ++i) {
        AtomicType* type = *i;
        Class* _class = dynamic_cast<Class*> (type);
        if(_class != 0)
            create_class_wrapper(_class);
    }
    for(std::vector<Function*>::iterator i = ns->functions.begin();
            i != ns->functions.end(); ++i) {
        create_function_wrapper(0, *i);
    }

    // create function list...
    out << "WrappedFunction " << modulename << "_global_functions[] = {\n";
    for(std::vector<Function*>::iterator i = ns->functions.begin();
            i != ns->functions.end(); ++i) {
        Function* function = *i;
        out << ind << "{ \"" << function->name << "\", &"
            << function->name << "_wrapper },\n";
    }
    out << ind << "{ 0, 0 }\n"
        << "};\n"
        << "\n";

    // create class list...
    std::ostringstream classlist;
    classlist << "WrappedClass " << modulename << "_classes[] = {\n";

    for(std::vector<AtomicType*>::iterator i = ns->types.begin();
            i != ns->types.end(); ++i) {
        AtomicType* type = *i;
        Class* _class = dynamic_cast<Class*> (type);
        if(_class == 0)
            continue;
        
        classlist << ind << "{ \"" << _class->name << "\", "
            << modulename << "_" << _class->name
            << "_methods },\n";
            
        out << "static WrappedFunction " << modulename << "_"
            << _class->name << "_methods[] = {\n";
        out << ind << "{ \"constructor\", &"
            << _class->name << "_" << "construct_wrapper },\n";
        for(std::vector<ClassMember*>::iterator i = _class->members.begin();
                i != _class->members.end(); ++i) {
            ClassMember* member = *i;
            if(member->visibility != ClassMember::PUBLIC)
                continue;
            Function* function = dynamic_cast<Function*> (member);
            if(!function || function->type != Function::FUNCTION)
                continue;

            out << ind << "{ \"" << function->name << "\", &"
                << _class->name << "_" << function->name << "_wrapper },\n";
        }
        out << "};\n"
            << "\n";
    }
    classlist << ind << "{ 0, 0 }\n";
    classlist << "};\n";
    out << classlist.str();
    out << "\n";
}

void
WrapperCreator::create_function_wrapper(Class* _class, Function* function)
{
    if(function->type == Function::CONSTRUCTOR)
        throw std::runtime_error("Constructors not supported yet");
    if(function->type == Function::DESTRUCTOR)
        throw std::runtime_error("Destructors not supported yet");
    
    out << "static int ";
    if(_class != 0) {
        out << _class->name << "_";
    }
    out << function->name << "_wrapper(HSQUIRRELVM v)\n"
        << "{\n";
    // avoid warning...
    if(_class == 0 && function->parameters.empty() 
            && function->return_type.is_void()) {
        out << ind << "(void) v;\n";
    }
    
    // eventually retrieve pointer to class
    if(_class != 0) {
        out << ind << _class->name << "* _this;\n";
        out << ind << "sq_getinstanceup(v, 1, (SQUserPointer*) &_this, 0);\n";
        out << ind << "assert(_this != 0);\n";
    }
    
    // declare and retrieve arguments
    size_t i = 0;
    for(std::vector<Parameter>::iterator p = function->parameters.begin();
            p != function->parameters.end(); ++p) {
        char argname[64];
        snprintf(argname, sizeof(argname), "arg%u", i);
        prepare_argument(p->type, i + 2, argname);
 
        ++i;
    }
    // call function
    out << ind << "\n";
    out << ind;
    if(!function->return_type.is_void()) {
        function->return_type.write_c_type(out);
        out << " return_value = ";
    }
    if(_class != 0) {
        out << "_this->";
    } else if(selected_namespace != "") {
        out << selected_namespace << "::";
    }
    out << function->name << "(";
    for(size_t i = 0; i < function->parameters.size(); ++i) {
        if(i != 0)
            out << ", ";
        out << "arg" << i;
    }
    out << ");\n";
    out << ind << "\n";
    // push return value back on stack and return
    if(function->return_type.is_void()) {
        out << ind << "return 0;\n";
    } else {
        push_to_stack(function->return_type, "return_value");
        out << ind << "return 1;\n";
    }
    out << "}\n";
    out << "\n";
}

void
WrapperCreator::prepare_argument(const Type& type, size_t index,
        const std::string& var)
{
    if(type.ref > 0 && type.atomic_type != StringType::instance())
        throw std::runtime_error("References not handled yet");
    if(type.pointer > 0)
        throw std::runtime_error("Pointers not handled yet");
    if(type.atomic_type == &BasicType::INT) {
        out << ind << "int " << var << ";\n";
        out << ind << "sq_getinteger(v, " << index << ", &" << var << ");\n";
    } else if(type.atomic_type == &BasicType::FLOAT) {
        out << ind << "float " << var << ";\n";
        out << ind << "sq_getfloat(v, " << index << ", &" << var << ");\n";
    } else if(type.atomic_type == &BasicType::BOOL) {
        out << ind << "SQBool " << var << ";\n";
        out << ind << "sq_getbool(v, " << index << ", &" << var << ");\n";
    } else if(type.atomic_type == StringType::instance()) {
        out << ind << "const char* " << var << ";\n";
        out << ind << "sq_getstring(v, " << index << ", &" << var << ");\n";
    } else {
        std::ostringstream msg;
        msg << "Type '" << type.atomic_type->name << "' not supported yet.";
        throw std::runtime_error(msg.str());
    }
}

void
WrapperCreator::push_to_stack(const Type& type, const std::string& var)
{
    if(type.ref > 0 && type.atomic_type != StringType::instance())
        throw std::runtime_error("References not handled yet");
    if(type.pointer > 0)
        throw std::runtime_error("Pointers not handled yet");
    out << ind;
    if(type.atomic_type == &BasicType::INT) {
        out << "sq_pushinteger(v, " << var << ");\n";
    } else if(type.atomic_type == &BasicType::FLOAT) {
        out << "sq_pushfloat(v, " << var << ");\n";
    } else if(type.atomic_type == &BasicType::BOOL) {
        out << "sq_pushbool(v, " << var << ");\n";
    } else if(type.atomic_type == StringType::instance()) {
        out << "sq_pushstring(v, " << var << ".c_str(), " 
            << var << ".size());\n";
    } else {
        std::ostringstream msg;
        msg << "Type '" << type.atomic_type->name << "' not supported yet.";
        throw std::runtime_error(msg.str());
    }
}

void
WrapperCreator::create_class_wrapper(Class* _class)
{
    create_class_destruct_function(_class);
    create_class_construct_function(_class);
    for(std::vector<ClassMember*>::iterator i = _class->members.begin();
            i != _class->members.end(); ++i) {
        ClassMember* member = *i;
        if(member->visibility != ClassMember::PUBLIC)
            continue;
        Function* function = dynamic_cast<Function*> (member);
        if(!function)
            continue;
        // don't wrap constructors and destructors (for now...)
        if(function->type != Function::FUNCTION)
            continue;
        create_function_wrapper(_class, function);
    }
}

void
WrapperCreator::create_class_construct_function(Class* _class)
{
    out << "static int " << _class->name << "_construct_wrapper(HSQUIRRELVM v)\n";
    out << "{\n";
    out << ind << _class->name << "* _this = new "
        << _class->name << "();\n";
    out << ind << "sq_setinstanceup(v, 1, _this);\n";
    out << ind << "sq_setreleasehook(v, 1, " 
        << _class->name << "_release_wrapper);\n";
    out << "\n";
    out << ind << "return 0;\n";
    out << "}\n";
    out << "\n";
}

void
WrapperCreator::create_class_destruct_function(Class* _class)
{
    out << "static int " << _class->name << "_release_wrapper(SQUserPointer ptr, int )\n"
        << "{\n"
        << ind << _class->name 
        << "* _this = reinterpret_cast<" << _class->name << "*> (ptr);\n"
        << ind << "delete _this;\n"
        << ind << "return 0;\n"
        << "}\n"
        << "\n";
}


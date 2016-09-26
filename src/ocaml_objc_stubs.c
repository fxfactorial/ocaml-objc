/* -*- objc -*- */

// Many namespace classes with alloc
#ifndef CAML_NAME_SPACE
#define CAML_NAME_SPACE
#endif

#import <objc/runtime.h>
#import <Foundation/Foundation.h>

#import <caml/mlvalues.h>
#import <caml/alloc.h>
#import <caml/memory.h>

#include "utils.h"
#include "dl_inspection.c"

CAMLprim value caml_objc_all_classes(void)
{
  CAMLparam0();
  CAMLreturn(make_list([NSObject classes]));
}

CAMLprim value caml_objc_properties(value class_name)
{
  CAMLparam1(class_name);

  Class class = objc_getClass(caml_strdup(String_val(class_name)));
  if (class == nil) CAMLreturn(Val_emptylist);
  CAMLreturn(make_list([class properties]));
}

CAMLprim value caml_objc_instance_variables(value class_name)
{
  CAMLparam1(class_name);

  Class class = objc_getClass(caml_strdup(String_val(class_name)));

  if (class == nil) CAMLreturn(Val_emptylist);
  CAMLreturn(make_list([class instanceVariables]));
}

CAMLprim value caml_objc_class_methods(value class_name)
{
  CAMLparam1(class_name);

  Class class = objc_getClass(caml_strdup(String_val(class_name)));
  if (class == nil) CAMLreturn(Val_emptylist);
  CAMLreturn(make_list([class classMethods]));
}

CAMLprim value caml_objc_instance_methods(value class_name)
{
  CAMLparam1(class_name);

  Class class = objc_getClass(caml_strdup(String_val(class_name)));
  if (class == nil) CAMLreturn(Val_emptylist);
  CAMLreturn(make_list([class instanceMethods]));
}

CAMLprim value caml_objc_protocols(value class_name)
{
  CAMLparam1(class_name);

  Class class = objc_getClass(caml_strdup(String_val(class_name)));
  if (class == nil) CAMLreturn(Val_emptylist);
  CAMLreturn(make_list([class protocols]));
}

#define Val_none Val_int(0)

CAMLprim value caml_objc_parent_class_hierarchy(value class_name)
{
  CAMLparam1(class_name);
  CAMLlocal1(some);

  Class class = objc_getClass(caml_strdup(String_val(class_name)));
  if (class == nil) CAMLreturn(Val_none);
  some = caml_alloc(1, 0);
  Store_field(some,
	      0,
	      caml_copy_string([[class parentClassHierarchy] UTF8String]));
  CAMLreturn(some);
}


CAMLprim value caml_objc_create_objc_object(value class_name)
{
  CAMLparam1(class_name);
  CAMLlocal2(some, wrapper);

  Class clazz = objc_getClass(caml_strdup(String_val(class_name)));

  if (clazz == nil) CAMLreturn(Val_none);

  id obj = [[clazz alloc] init];

  if (obj == nil) CAMLreturn(Val_none);

  wrapper = caml_alloc_small(1, Abstract_tag);
  some = caml_alloc(1, 0);

  Store_field(wrapper, 0, (value)obj);
  Store_field(some, 0, wrapper);
  CAMLreturn(some);
}

CAMLprim value caml_objc_create_nsstring(value s)
{
  CAMLparam1(s);
  CAMLlocal1(wrapper);
  wrapper = caml_alloc_small(1, Abstract_tag);
  Store_field(wrapper, 0, (value)@(caml_strdup(String_val(s))));

  CAMLreturn(wrapper);
}

CAMLprim value caml_objc_obj_description(value obj)
{
  CAMLparam1(obj);
  CAMLreturn(caml_copy_string([[(__bridge id)(void*)Field(obj, 0)
					     description] UTF8String]));
}

// Make more robust
CAMLprim value caml_objc_call_method(value class, value selector, value args)
{
  CAMLparam3(class, selector, args);
  CAMLlocal2(some, wrapper);

  size_t arg_len = Wosize_val(args);

  id object_instance = (__bridge id)(void*)Field(class, 0);
  NSString *as_string = @(caml_strdup(String_val(selector)));

  SEL selector_ = NSSelectorFromString(as_string);

  NSMethodSignature *sig =
    [object_instance methodSignatureForSelector:selector_];

  NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:sig];

  invocation.selector = selector_;
  invocation.target = object_instance;

  if (arg_len != 0) {
    for (size_t i = 1; i <= arg_len; i++) {
      id arg = (__bridge id)(void*)Field(args, i);
      [invocation setArgument:&arg atIndex:i + 2];
    }
  }

  id result = nil;
  [invocation invoke];
  [invocation getReturnValue:&result];

  if (result == nil)
    CAMLreturn(Val_none);

  some = caml_alloc(1, 0);
  wrapper = caml_alloc_small(1, Abstract_tag);

  Store_field(wrapper, 0, (value)result);
  Store_field(some, 0, wrapper);

  CAMLreturn(some);
}

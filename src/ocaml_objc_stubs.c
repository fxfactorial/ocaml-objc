/* -*- objc -*- */

// Many namespace classes with alloc
#define CAML_NAME_SPACE

//
//  NSObject+DLIntrospection.m
//  DLIntrospection
//
//  Created by Denis Lebedev on 12/27/12.
//  Copyright (c) 2012 Denis Lebedev. All rights reserved.
//
#import <objc/runtime.h>
#import <Foundation/Foundation.h>

@interface NSObject (DLIntrospection)

+ (NSArray *)classes;
+ (NSArray *)properties;
+ (NSArray *)instanceVariables;
+ (NSArray *)classMethods;
+ (NSArray *)instanceMethods;
+ (NSArray *)protocols;
+ (NSDictionary *)descriptionForProtocol:(Protocol *)proto;
+ (NSString *)parentClassHierarchy;
@end

@interface NSString (DLIntrospection)
+ (NSString *)decodeType:(const char *)cString;
@end

@implementation NSString (DLIntrospection)

//https://developer.apple.com/library/mac/#documentation/Cocoa/Conceptual/ObjCRuntimeGuide/Articles/ocrtTypeEncodings.html
+ (NSString *)decodeType:(const char *)cString {
  if (!strcmp(cString, @encode(id))) return @"id";
  if (!strcmp(cString, @encode(void))) return @"void";
  if (!strcmp(cString, @encode(float))) return @"float";
  if (!strcmp(cString, @encode(int))) return @"int";
  if (!strcmp(cString, @encode(BOOL))) return @"BOOL";
  if (!strcmp(cString, @encode(char *))) return @"char *";
  if (!strcmp(cString, @encode(double))) return @"double";
  if (!strcmp(cString, @encode(Class))) return @"class";
  if (!strcmp(cString, @encode(SEL))) return @"SEL";
  if (!strcmp(cString, @encode(unsigned int))) return @"unsigned int";

  //@TODO: do handle bitmasks
  NSString *result = [NSString stringWithCString:cString encoding:NSUTF8StringEncoding];
  if ([[result substringToIndex:1] isEqualToString:@"@"] &&
      [result rangeOfString:@"?"].location == NSNotFound) {
    result = [[result substringWithRange:NSMakeRange(2, result.length - 3)]
	       stringByAppendingString:@"*"];
  } else
    if ([[result substringToIndex:1] isEqualToString:@"^"]) {
      result = [NSString stringWithFormat:@"%@ *",
		     [NSString decodeType:[[result substringFromIndex:1]
					    cStringUsingEncoding:NSUTF8StringEncoding]]];
    }
  return result;
}

@end

static void getSuper(Class class, NSMutableString *result) {
  [result appendFormat:@" -> %@", NSStringFromClass(class)];
  if ([class superclass]) { getSuper([class superclass], result); }
}


@implementation NSObject (DLIntrospection)

+ (NSArray *)classes {
  unsigned int classesCount;
  Class *classes = objc_copyClassList(&classesCount);
  NSMutableArray *result = [NSMutableArray array];
  for (unsigned int i = 0 ; i < classesCount; i++) {
    [result addObject:NSStringFromClass(classes[i])];
  }
  return [result sortedArrayUsingSelector:@selector(compare:)];
}

+ (NSArray *)classMethods {
  return [self methodsForClass:object_getClass([self class]) typeFormat:@"+"];
}

+ (NSArray *)instanceMethods {
  return [self methodsForClass:[self class] typeFormat:@"-"];
}

+ (NSArray *)properties {
  unsigned int outCount;
  objc_property_t *properties = class_copyPropertyList([self class], &outCount);
  NSMutableArray *result = [NSMutableArray array];
  for (unsigned int i = 0; i < outCount; i++) {
    [result addObject:[self formattedPropery:properties[i]]];
  }
  free(properties);
  return result.count ? [result copy] : nil;
}

+ (NSArray *)instanceVariables {
  unsigned int outCount;
  Ivar *ivars = class_copyIvarList([self class], &outCount);
  NSMutableArray *result = [NSMutableArray array];
  for (unsigned int i = 0; i < outCount; i++) {
    NSString *type = [NSString decodeType:ivar_getTypeEncoding(ivars[i])];
    NSString *name = [NSString stringWithCString:ivar_getName(ivars[i])
					encoding:NSUTF8StringEncoding];
    NSString *ivarDescription = [NSString stringWithFormat:@"%@ %@", type, name];
    [result addObject:ivarDescription];
  }
  free(ivars);
  return result.count ? [result copy] : nil;
}

+ (NSArray *)protocols {
  unsigned int outCount;
  Protocol * const *protocols = class_copyProtocolList([self class], &outCount);

  NSMutableArray *result = [NSMutableArray array];
  for (unsigned int i = 0; i < outCount; i++) {
    unsigned int adoptedCount;
    Protocol * const *adotedProtocols = protocol_copyProtocolList(protocols[i],
								  &adoptedCount);
    NSString *protocolName = [NSString stringWithCString:protocol_getName(protocols[i])
						encoding:NSUTF8StringEncoding];

    NSMutableArray *adoptedProtocolNames = [NSMutableArray array];
    for (unsigned int idx = 0; idx < adoptedCount; idx++) {
      [adoptedProtocolNames
	addObject:[NSString stringWithCString:protocol_getName(adotedProtocols[idx])
				     encoding:NSUTF8StringEncoding]];
    }
    NSString *protocolDescription = protocolName;

    if (adoptedProtocolNames.count) {
      protocolDescription = [NSString stringWithFormat:@"%@ <%@>",
				      protocolName,
				      [adoptedProtocolNames componentsJoinedByString:@", "]];
    }
    [result addObject:protocolDescription];
    //free(adotedProtocols);
  }
  //free((__bridge void *)(*protocols));
  return result.count ? [result copy] : nil;
}

+ (NSDictionary *)descriptionForProtocol:(Protocol *)proto {
  NSMutableDictionary *methodsAndProperties = [NSMutableDictionary dictionary];

  NSArray *requiredMethods =
    [[[self class] formattedMethodsForProtocol:proto required:YES instance:NO]
      arrayByAddingObjectsFromArray:[[self class] formattedMethodsForProtocol:proto
								     required:YES
								     instance:YES]];

  NSArray *optionalMethods =
    [[[self class]
       formattedMethodsForProtocol:proto
			  required:NO
			  instance:NO]
			       arrayByAddingObjectsFromArray:[[self class]
							       formattedMethodsForProtocol:proto
										  required:NO
										  instance:YES]];

  unsigned int propertiesCount;
  NSMutableArray *propertyDescriptions = [NSMutableArray array];
  objc_property_t *properties = protocol_copyPropertyList(proto, &propertiesCount);
  for (unsigned int i = 0; i < propertiesCount; i++) {
    [propertyDescriptions addObject:[self formattedPropery:properties[i]]];
  }

  if (requiredMethods.count) {
    [methodsAndProperties setObject:requiredMethods forKey:@"@required"];
  }
  if (optionalMethods.count) {
    [methodsAndProperties setObject:optionalMethods forKey:@"@optional"];
  } if (propertyDescriptions.count) {
    [methodsAndProperties setObject:[propertyDescriptions copy] forKey:@"@properties"];
  }

  free(properties);
  return methodsAndProperties.count ? [methodsAndProperties copy ] : nil;
}

+ (NSString *)parentClassHierarchy {
  NSMutableString *result = [NSMutableString string];
  getSuper([self class], result);
  return result;
}

#pragma mark - Private

+ (NSArray *)methodsForClass:(Class)class typeFormat:(NSString *)type {
  unsigned int outCount;
  Method *methods = class_copyMethodList(class, &outCount);
  NSMutableArray *result = [NSMutableArray array];
  for (unsigned int i = 0; i < outCount; i++) {
    NSString *methodDescription = [NSString stringWithFormat:@"%@ (%@)%@",
					    type,
					[NSString decodeType:method_copyReturnType(methods[i])],
					    NSStringFromSelector(method_getName(methods[i]))];

    NSInteger args = method_getNumberOfArguments(methods[i]);
    NSMutableArray *selParts = [[methodDescription componentsSeparatedByString:@":"] mutableCopy];
    NSInteger offset = 2; //1-st arg is object (@), 2-nd is SEL (:)

    for (int idx = offset; idx < args; idx++) {
      NSString *returnType = [NSString decodeType:method_copyArgumentType(methods[i], idx)];
      selParts[idx - offset] = [NSString stringWithFormat:@"%@:(%@)arg%d",
					 selParts[idx - offset],
					 returnType,
					 idx - 2];
    }
    [result addObject:[selParts componentsJoinedByString:@" "]];
  }
  free(methods);
  return result.count ? [result copy] : nil;
}

+ (NSArray *)formattedMethodsForProtocol:(Protocol *)proto required:(BOOL)required instance:(BOOL)instance {
  unsigned int methodCount;
  struct objc_method_description *methods = protocol_copyMethodDescriptionList(proto, required, instance, &methodCount);
  NSMutableArray *methodsDescription = [NSMutableArray array];
  for (unsigned int i = 0; i < methodCount; i++) {
    [methodsDescription addObject:
			  [NSString stringWithFormat:@"%@ (%@)%@",
				    instance ? @"-" : @"+",
				    @"void",
				    NSStringFromSelector(methods[i].name)]];
  }

  free(methods);
  return  [methodsDescription copy];
}

+ (NSString *)formattedPropery:(objc_property_t)prop {
  unsigned int attrCount;
  objc_property_attribute_t *attrs = property_copyAttributeList(prop, &attrCount);
  NSMutableDictionary *attributes = [NSMutableDictionary dictionary];
  for (unsigned int idx = 0; idx < attrCount; idx++) {
    NSString *name = [NSString stringWithCString:attrs[idx].name encoding:NSUTF8StringEncoding];
    NSString *value = [NSString stringWithCString:attrs[idx].value encoding:NSUTF8StringEncoding];
    [attributes setObject:value forKey:name];
  }
  free(attrs);
  NSMutableString *property = [NSMutableString stringWithFormat:@"@property "];
  NSMutableArray *attrsArray = [NSMutableArray array];

  //https://developer.apple.com/library/mac/#documentation/Cocoa/Conceptual/ObjCRuntimeGuide/Articles/ocrtPropertyIntrospection.html#//apple_ref/doc/uid/TP40008048-CH101-SW5
  [attrsArray addObject:[attributes objectForKey:@"N"] ? @"nonatomic" : @"atomic"];

  if ([attributes objectForKey:@"&"]) {
    [attrsArray addObject:@"strong"];
  } else if ([attributes objectForKey:@"C"]) {
    [attrsArray addObject:@"copy"];
  } else if ([attributes objectForKey:@"W"]) {
    [attrsArray addObject:@"weak"];
  } else {
    [attrsArray addObject:@"assign"];
  }
  if ([attributes objectForKey:@"R"]) {[attrsArray addObject:@"readonly"];}
  if ([attributes objectForKey:@"G"]) {
    [attrsArray addObject:[NSString stringWithFormat:@"getter=%@",
			    [attributes objectForKey:@"G"]]];
  }
  if ([attributes objectForKey:@"S"]) {
    [attrsArray addObject:[NSString stringWithFormat:@"setter=%@",
			    [attributes objectForKey:@"G"]]];
  }

  [property appendFormat:@"(%@) %@ %@",
	    [attrsArray componentsJoinedByString:@", "],
    [NSString decodeType:[[attributes objectForKey:@"T"]
			   cStringUsingEncoding:NSUTF8StringEncoding]],
	    [NSString stringWithCString:property_getName(prop)
			       encoding:NSUTF8StringEncoding]];
  return [property copy];
}
@end

#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>

CAMLprim value make_list(NSArray * _Nonnull names)
{
  CAMLparam0();
  CAMLlocal2(list, cons);

  list = Val_emptylist;

  for (NSString *s in names) {
    cons = caml_alloc(2, 0);
    Store_field(cons, 0, caml_copy_string([s UTF8String]));
    Store_field(cons, 1, list);
    list = cons;
  }
  CAMLreturn(list);
}

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
  CAMLreturn(caml_copy_string([[(__bridge id)(void*)Field(obj, 0) description] UTF8String]));
}

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

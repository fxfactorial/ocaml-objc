/* -*- objc -*- */
#ifndef CAML_NAME_SPACE
#define CAML_NAME_SPACE
#endif

#import <objc/runtime.h>
#import <Foundation/Foundation.h>

#import <caml/mlvalues.h>
#import <caml/alloc.h>
#import <caml/memory.h>

CAMLprim value Val_some(value);
CAMLprim value make_list(NSArray * _Nonnull);


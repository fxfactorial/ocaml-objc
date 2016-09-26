/* -*- objc -*- */
#ifndef CAML_NAME_SPACE
#define CAML_NAME_SPACE
#endif

#import <objc/runtime.h>
#import <Foundation/Foundation.h>

#import <caml/mlvalues.h>
#import <caml/alloc.h>
#import <caml/memory.h>

CAMLprim value Val_some(value v)
{
  CAMLparam1(v);
  CAMLlocal1(some);
  some = caml_alloc_small(1, 0);
  Store_field(some, 0, v);
  CAMLreturn(some);
}

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

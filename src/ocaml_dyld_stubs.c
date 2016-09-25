#include <stdio.h>
#include <mach-o/dyld.h>
#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>

#define Val_none Val_int(0)

CAMLprim value caml_dyld_image_count(void)
{
  CAMLparam0();
  CAMLreturn(Val_int(_dyld_image_count()));
}

CAMLprim value caml_dyld_image_name(value image_spot)
{
  CAMLparam1(image_spot);
  CAMLlocal1(some);

  const char *s = _dyld_get_image_name(Int_val(image_spot));
  if (s == NULL)
    CAMLreturn(Val_none);
  some = caml_alloc(1, 0);
  Store_field(some, 0, caml_copy_string(s));
  CAMLreturn(some);
}

/* __attribute__((constructor)) */
/* void test(void) */
/* { */
/*   printf("Test: %lu, %lu, %lu\n", */
/* 	 _dyld_get_image_vmaddr_slide(0), */
/* 	 _dyld_get_image_vmaddr_slide(10), */
/* 	 _dyld_get_image_vmaddr_slide(12)); */

/* } */

CAMLprim value caml_dyld_image_slide(value image_spot)
{
  CAMLparam1(image_spot);
  CAMLreturn(caml_copy_nativeint(_dyld_get_image_vmaddr_slide(Int_val(image_spot))));
}

/* CAMLprim value caml_dyld_image_version(value image_name) */
/* { */
/*   CAMLparam1(image_name); */
/*   CAMLlocal1(some); */

/*   int32_t version = */
/*     NSVersionOfRunTimeLibrary(caml_strdup(String_val(image_name))); */
/*   printf("Version was:%d\n", version); */
/*   if (version == -1) */
/*     CAMLreturn(Val_none); */
/*   some = caml_alloc(1, 0); */
/*   Store_field(some, 0, Int_val(version)); */
/*   CAMLreturn(some); */
/* } */

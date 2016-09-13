(** OCaml interface to Objective-C *)

(** Introspection of Objective-C objects, an empty list means the
    class was not known by the objective-c runtime *)
module Introspect : sig

  (** A list of all known Objective-C objects *)
  val classes : unit -> string list

  (** All @properties of an object *)
  val properties : string -> string list

  (** All instance variables of an object *)
  val instance_variables : string -> string list

  (** All class methods of an object *)
  val class_methods : string -> string list

  (** All instance methods of an object *)
  val instance_methods : string -> string list

  (** All the protocols that the Objective-C object conforms to *)
  val protocols : string -> string list

  (** Hierarchy of the Objective-C object *)
  val parent_class_hierarchy : string -> string option

end

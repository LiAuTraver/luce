// This file is intentionally left without a include guard.

#if defined(Add) || defined(Xor) || defined(Or) || defined(And) ||             \
    defined(min) || defined(max)
#  error "Bad user. Bad code."
#endif

#ifdef AC_UNDEF_YOUR_FXXKING_MACRO
#  undef AC_UNDEF_YOUR_FXXKING_MACRO

#  pragma push_macro("R")
#  pragma push_macro("I")
#  pragma push_macro("S")
#  pragma push_macro("B")
#  pragma push_macro("U")
#  pragma push_macro("J")
#  pragma push_macro("INST")
#  pragma push_macro("INST_IMPL")
#  pragma push_macro("CONCAT_HELPER")
#  undef R
#  undef I
#  undef S
#  undef B
#  undef U
#  undef J
#  undef INST
#  undef INST_IMPL
#  undef CONCAT_HELPER

#  define INST_IMPL(_name_, _fmt_)                                             \
    struct _name_ : IInstruction, mixin::_fmt_ {                               \
    protected:                                                                 \
      virtual auto asmStr() const noexcept -> string_type override;            \
      using base_type = IInstruction;                                          \
      using base_type::base_type;                                              \
                                                                               \
    public:                                                                    \
      virtual ExecutionStatus execute(Icpu *) const override;                  \
    }
#  define CONCAT_HELPER(_name_, _fmt_) _name_##_fmt_
#  define INST(_name_, _fmt_) INST_IMPL(_name_, CONCAT_HELPER(_fmt_, Format))
#elif defined(AC_RESTORE_YOUR_FXXKING_MACRO)
#  undef AC_RESTORE_YOUR_FXXKING_MACRO
#  undef INST
#  undef CONCAT_HELPER
#  undef INST_IMPL
#  pragma pop_macro("INST")
#  pragma pop_macro("INST_IMPL")
#  pragma pop_macro("CONCAT_HELPER")
#  pragma pop_macro("J")
#  pragma pop_macro("U")
#  pragma pop_macro("B")
#  pragma pop_macro("S")
#  pragma pop_macro("I")
#  pragma pop_macro("R")
#elif defined(__INTELLISENSE__)
// pass
#else
#  error                                                                       \
      "AC_UNDEF_YOUR_FXXKING_MACRO or AC_REST_YOUR_FXXKING_MACRO is not defined"
#endif

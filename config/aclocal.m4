# AC_C_WCHAR_T_UNSIGNED
# ------------------
AC_DEFUN([AC_C_WCHAR_T_UNSIGNED],
[AC_CACHE_CHECK(whether wchar_t is unsigned, ac_cv_c_wchar_t_unsigned,
[AC_COMPILE_IFELSE([AC_LANG_BOOL_COMPILE_TRY([AC_INCLUDES_DEFAULT([])],
                                             [((wchar_t) -1) < 0])],
                   ac_cv_c_wchar_t_unsigned=no, ac_cv_c_wchar_t_unsigned=yes)])
if test $ac_cv_c_wchar_t_unsigned = yes; then
  WCHAR_T_UNSIGNED=1          
else
  WCHAR_T_UNSIGNED=0          
fi
AC_SUBST(WCHAR_T_UNSIGNED)dnl
])# AC_C_WCHAR_T_UNSIGNED


 struct Cyc_Getopt_option{ struct _tagged_arr name; int has_arg; int* flag; int
val; } ; extern int Cyc_Getopt_getopt_long( int __argc, struct _tagged_arr
__argv, struct _tagged_arr __shortopts, struct _tagged_arr __longopts, int*
__longind); extern int Cyc_Getopt_getopt_long_only( int __argc, struct
_tagged_arr __argv, struct _tagged_arr __shortopts, struct _tagged_arr
__longopts, int* __longind); extern int Cyc_Getopt__getopt_internal( int __argc,
struct _tagged_arr __argv, struct _tagged_arr __shortopts, struct _tagged_arr
__longopts, int* __longind, int __long_only); int Cyc_Getopt_getopt_long( int
argc, struct _tagged_arr argv, struct _tagged_arr options, struct _tagged_arr
long_options, int* opt_index){ return Cyc_Getopt__getopt_internal( argc, argv,
options, long_options, opt_index, 0);} int Cyc_Getopt_getopt_long_only( int argc,
struct _tagged_arr argv, struct _tagged_arr options, struct _tagged_arr
long_options, int* opt_index){ return Cyc_Getopt__getopt_internal( argc, argv,
options, long_options, opt_index, 1);}

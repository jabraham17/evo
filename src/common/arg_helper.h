
#ifndef COMMON_ARG_HELPER_H_
#define COMMON_ARG_HELPER_H_

// empty str, no args just bool
// one colon, 1 required arg
// two colo, 1 optional arg
#define _SHORT_WITH_STR_str(_char, _variable) #_char ":"
#define _SHORT_WITH_INT_str(_char, _variable) #_char ":"
#define _SHORT_WITH_FP_str(_char, _variable) #_char ":"
#define _SHORT_WITH_BOOL_str(_char, _variable) #_char ""

#define _SHORT_WITH_STR_act(_char, _variable)                                  \
    case #_char[0]:                                                            \
        _variable = optarg;                                                    \
        break;

#define _SHORT_WITH_INT_act(_char, _variable)                                  \
    case #_char[0]:                                                            \
        _variable = atoll(optarg);                                             \
        break;

#define _SHORT_WITH_FP_act(_char, _variable)                                   \
    case #_char[0]:                                                            \
        _variable = atof(optarg);                                              \
        break;

#define _SHORT_WITH_BOOL_act(_char, _variable)                                 \
    case #_char[0]:                                                            \
        _variable = 1;                                                         \
        break;

#define SHORT_MAKE_ARGS(_argc, _argv, _ARG_LIST)                               \
    do {                                                                       \
        int _c;                                                                \
        opterr = 1;                                                            \
        while((_c = getopt(                                                    \
                   _argc,                                                      \
                   _argv,                                                      \
                   _ARG_LIST(                                                  \
                       _SHORT_WITH_STR_str,                                    \
                       _SHORT_WITH_INT_str,                                    \
                       _SHORT_WITH_FP_str,                                     \
                       _SHORT_WITH_BOOL_str))) != -1) {                        \
            switch(_c) {                                                       \
                _ARG_LIST(                                                     \
                    _SHORT_WITH_STR_act,                                       \
                    _SHORT_WITH_INT_act,                                       \
                    _SHORT_WITH_FP_act,                                        \
                    _SHORT_WITH_BOOL_act)                                      \
                case '?':                                                      \
                default: exit(1);                                              \
            }                                                                  \
        }                                                                      \
    } while(0)

#endif


#define DL_CONCAT_OR_APPEND(head1, head2)                                      \
    DL_CONCAT_OR_APPEND2(head1, head2, prev, next)

#define DL_CONCAT_OR_APPEND2(head1, head2, prev, next)                         \
    do {                                                                       \
        LDECLTYPE(head1) _tmp;                                                 \
        if(head2) {                                                            \
            if(!((head2)->prev)) {                                             \
                (head2)->prev = (head2);                                       \
            }                                                                  \
            if(head1) {                                                        \
                if(!((head1)->prev)) {                                         \
                    (head1)->prev = (head1);                                   \
                }                                                              \
                UTLIST_CASTASGN(_tmp, (head2)->prev);                          \
                (head2)->prev = (head1)->prev;                                 \
                (head1)->prev->next = (head2);                                 \
                UTLIST_CASTASGN((head1)->prev, _tmp);                          \
            } else {                                                           \
                (head1) = (head2);                                             \
            }                                                                  \
        }                                                                      \
    } while(0)

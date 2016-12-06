//
// Created by chao on 02/12/2016.
//

#include "error.h"

namespace eschao {

char err_desc[MAX_DESC_LENGTH + 1];

int check_err_desc_len(int len) {
    if (len > MAX_DESC_LENGTH) {
        len = MAX_DESC_LENGTH;
    }

    err_desc[len + 1] = '\0';
    return len;
}

}

#include <assert.h>
#include <stdio.h>
#include "omega/capability.h"

int main(void) {
    omega_capability_t root;
    omega_capability_t child;

    /* Init */
    assert(omega_capability_init(&root, 10,
        OMEGA_CAP_RIGHT_READ | OMEGA_CAP_RIGHT_WRITE) == 0);
    assert(root.valid == 1);
    assert(omega_capability_has(&root, OMEGA_CAP_RIGHT_READ));
    assert(omega_capability_has(&root, OMEGA_CAP_RIGHT_WRITE));

    /* Derivar con subconjunto */
    assert(omega_capability_derive(&root, &child, OMEGA_CAP_RIGHT_READ) == 0);
    assert(omega_capability_has(&child, OMEGA_CAP_RIGHT_READ));
    assert(!omega_capability_has(&child, OMEGA_CAP_RIGHT_WRITE));

    /* Derivar con derechos en exceso falla */
    omega_capability_t bad;
    assert(omega_capability_derive(&root, &bad,
        OMEGA_CAP_RIGHT_READ | OMEGA_CAP_RIGHT_EXEC) == -1);

    /* Revocar */
    omega_capability_revoke(&child);
    assert(child.valid == 0);
    assert(!omega_capability_has(&child, OMEGA_CAP_RIGHT_READ));

    printf("test_capability: OK\n");
    return 0;
}

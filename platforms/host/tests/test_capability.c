#include <assert.h>
#include <stdio.h>
#include "omega/capability.h"

int main(void) {
    omega_capability_t root;
    omega_capability_t child;
    omega_capability_t delegated;
    omega_capability_t delegated_no_grant;
    omega_capability_t no_grant_source;

    /* Init con owner */
    assert(omega_capability_init(&root, 10, 1,
        OMEGA_CAP_RIGHT_READ | OMEGA_CAP_RIGHT_WRITE | OMEGA_CAP_RIGHT_GRANT) == 0);
    assert(root.valid == 1);
    assert(root.owner_id == 1);
    assert(omega_capability_has(&root, OMEGA_CAP_RIGHT_READ));
    assert(omega_capability_has(&root, OMEGA_CAP_RIGHT_WRITE));
    assert(omega_capability_has(&root, OMEGA_CAP_RIGHT_GRANT));

    /* Derivar con subconjunto */
    assert(omega_capability_derive(&root, &child,
        OMEGA_CAP_RIGHT_READ | OMEGA_CAP_RIGHT_GRANT) == 0);
    assert(child.owner_id == 1);  /* Mismo owner que la madre */
    assert(omega_capability_has(&child, OMEGA_CAP_RIGHT_READ));
    assert(!omega_capability_has(&child, OMEGA_CAP_RIGHT_WRITE));

    /* Derivar con derechos en exceso falla */
    omega_capability_t bad;
    assert(omega_capability_derive(&root, &bad,
        OMEGA_CAP_RIGHT_READ | OMEGA_CAP_RIGHT_EXEC) == -1);

    /* Delegación con GRANT */
    assert(omega_capability_delegate(&root, 2, &delegated) == 0);
    assert(delegated.valid == 1);
    assert(delegated.owner_id == 2);
    assert(delegated.object_id == 10);
    assert(delegated.parent_owner == 1);
    /* La delegada no conserva GRANT */
    assert(!omega_capability_has(&delegated, OMEGA_CAP_RIGHT_GRANT));
    /* Pero conserva los demás */
    assert(omega_capability_has(&delegated, OMEGA_CAP_RIGHT_READ));
    assert(omega_capability_has(&delegated, OMEGA_CAP_RIGHT_WRITE));

    /* Delegación sin GRANT falla */
    assert(omega_capability_init(&no_grant_source, 20, 5,
        OMEGA_CAP_RIGHT_READ) == 0);
    assert(omega_capability_delegate(&no_grant_source, 6,
        &delegated_no_grant) == -1);

    /* Revocación */
    omega_capability_revoke(&child);
    assert(child.valid == 0);
    assert(!omega_capability_has(&child, OMEGA_CAP_RIGHT_READ));

    /* Igualdad */
    omega_capability_t c1, c2;
    assert(omega_capability_init(&c1, 99, 7, OMEGA_CAP_RIGHT_READ) == 0);
    assert(omega_capability_init(&c2, 99, 7, OMEGA_CAP_RIGHT_READ) == 0);
    /* Nota: generation difiere, pero equal() no la compara */
    assert(omega_capability_equal(&c1, &c2) == 1);

    printf("test_capability: OK\n");
    return 0;
}

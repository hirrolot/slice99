#ifndef ASSERT_ALGEBRAIC_H
#define ASSERT_ALGEBRAIC_H

#include <assert.h>

#define ASSERT_IMPLICATION(x, y)                                                                   \
    do {                                                                                           \
        if (x) {                                                                                   \
            assert(y);                                                                             \
        }                                                                                          \
    } while (0)

#define ASSERT_BIDIRECTIONAL_IMPLICATION(x, y)                                                     \
    do {                                                                                           \
        ASSERT_IMPLICATION(x, y);                                                                  \
        ASSERT_IMPLICATION(y, x);                                                                  \
    } while (0)

#define ASSERT_REFLEXIVE(f, x) assert(f(x, x))

#define ASSERT_IRREFLEXIVE(f, x) assert(!f(x, x))

#define ASSERT_SYMMETRIC(f, x, y) ASSERT_BIDIRECTIONAL_IMPLICATION(f(x, y), f(y, x))

#define ASSERT_ANTISYMMETRIC(f, eq, x, y) ASSERT_IMPLICATION(f(x, y) && f(y, x), eq(x, y))

#define ASSERT_ASYMMETRIC(f, x, y) ASSERT_IMPLICATION(f(x, y), !f(y, x))

#define ASSERT_TRANSITIVE(f, x, y, z) ASSERT_IMPLICATION(f(x, y) && f(y, z), f(x, z))

#define ASSERT_ANTITRANSITIVE(f, x, y, z) ASSERT_IMPLICATION(f(x, y) && f(y, z), !f(x, z))

#define ASSERT_CONNEXIVE(f, x, y) assert(f(x, y) || f(y, x))

#define ASSERT_SEMICONNEXIVE(f, eq, x, y) assert(f(x, y) || f(y, x) || eq(x, y))

#define ASSERT_BIJECTIVE(f, g, eq, x) assert(eq(g(f(x)), x))

#define ASSERT_INVOLUTIVE(f, eq, x) ASSERT_BIJECTIVE(f, f, eq, x)

#define ASSERT_IDEMPOTENT(f, eq, x) assert(eq(f(x, x), x))

#define ASSERT_COMMUTATIVE(f, eq, x, y) assert(eq(f(x, y), f(y, x)))

#define ASSERT_ASSOCIATIVE(f, eq, x, y, z) assert(eq(f(f(x, y), z), f(x, f(y, z))))

#define ASSERT_LEFT_DISTRIBUTIVE(f1, f2, eq, x, y, z)                                              \
    assert(eq(f1(x, f2(y, z)), f2(f1(x, y), f1(x, z))))

#define ASSERT_RIGHT_DISTRIBUTIVE(f1, f2, eq, x, y, z)                                             \
    assert(eq(f1(f2(y, z), x), f2(f1(y, x), f1(z, x))))

#define ASSERT_DISTRIBUTIVE(f1, f2, eq, x, y, z)                                                   \
    do {                                                                                           \
        ASSERT_LEFT_DISTRIBUTIVE(f1, f2, eq, x, y, z);                                             \
        ASSERT_RIGHT_DISTRIBUTIVE(f1, f2, eq, x, y, z);                                            \
    } while (0)

#define ASSERT_EQUIVALENCE(f, x, y, z)                                                             \
    do {                                                                                           \
        ASSERT_REFLEXIVE(f, x);                                                                    \
        ASSERT_SYMMETRIC(f, x, y);                                                                 \
        ASSERT_TRANSITIVE(f, x, y, z);                                                             \
    } while (0)

#define ASSERT_LEFT_IDENTITY(f, eq, e, x) assert(eq(f(e, x), x))

#define ASSERT_RIGHT_IDENTITY(f, eq, e, x) assert(eq(f(x, e), x))

#define ASSERT_IDENTITY(f, eq, e, x)                                                               \
    do {                                                                                           \
        ASSERT_LEFT_IDENTITY(f, eq, e, x);                                                         \
        ASSERT_RIGHT_IDENTITY(f, eq, e, x);                                                        \
    } while (0)

#define ASSERT_LEFT_ZERO(f, eq, z, x) assert(eq(f(z, x), z))

#define ASSERT_RIGHT_ZERO(f, eq, z, x) assert(eq(f(x, z), z))

#define ASSERT_ZERO(f, eq, z, x)                                                                   \
    do {                                                                                           \
        ASSERT_LEFT_ZERO(f, eq, z, x);                                                             \
        ASSERT_RIGHT_ZERO(f, eq, z, x);                                                            \
    } while (0)

#define ASSERT_PREORDER(f, x, y, z)                                                                \
    do {                                                                                           \
        ASSERT_REFLEXIVE(f, x);                                                                    \
        ASSERT_TRANSITIVE(f, x, y, z);                                                             \
    } while (0)

#define ASSERT_PARTIAL_ORDER(f, eq, x, y, z)                                                       \
    do {                                                                                           \
        ASSERT_PREORDER(f, x, y, z);                                                               \
        ASSERT_ANTISYMMETRIC(f, eq, x, y);                                                         \
    } while (0)

#define ASSERT_STRICT_PARTIAL_ORDER(f, x, y, z)                                                    \
    do {                                                                                           \
        ASSERT_TRANSITIVE(f, x, y, z);                                                             \
        ASSERT_ASYMMETRIC(f, x, y);                                                                \
    } while (0)

#define ASSERT_TOTAL_ORDER(f, eq, x, y, z)                                                         \
    do {                                                                                           \
        ASSERT_ANTISYMMETRIC(f, eq, x, y);                                                         \
        ASSERT_TRANSITIVE(f, x, y, z);                                                             \
        ASSERT_CONNEXIVE(f, x, y);                                                                 \
    } while (0)

#define ASSERT_STRICT_TOTAL_ORDER(f, eq, x, y, z)                                                  \
    do {                                                                                           \
        ASSERT_ASYMMETRIC(f, x, y);                                                                \
        ASSERT_TRANSITIVE(f, x, y, z);                                                             \
        ASSERT_SEMICONNEXIVE(f, eq, x, y);                                                         \
    } while (0)

#endif // ASSERT_ALGEBRAIC_H

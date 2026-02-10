#include "unity.h"
#include "median.h"

// Ces fonctions sont appelées avant et après chaque test
void setUp(void) {
    // Initialisation si nécessaire
}

void tearDown(void) {
    // Nettoyage si nécessaire
}

void test_median_basic_filtering(void) {
    // Le filtre a une longueur de 9 (MEDIAN_BUFFER_LENGTH)
    // On le remplit avec des valeurs stables
    for(int i=0; i<8; i++) {
        median_get_median(10);
    }
    
    // On ajoute une valeur très élevée (bruit/ausreißer)
    uint32_t result = median_get_median(100);
    
    // Le résultat ne devrait pas être 100, car le 100 est filtré
    // Dans l'implémentation, il y a aussi un lissage : (4*last + 1*current)/5
    // Donc le résultat devrait être proche de 10
    TEST_ASSERT_LESS_THAN(50, result);
    TEST_ASSERT_GREATER_THAN(5, result);
}

void test_median_sequence(void) {
    // On teste si le filtre réagit bien à une rampe
    median_get_median(10);
    median_get_median(11);
    median_get_median(12);
    median_get_median(13);
    uint32_t result = median_get_median(14); // 5ème élément
    
    // Comme le buffer n'est pas encore plein de 14, 
    // et avec le lissage, la valeur doit monter doucement
    TEST_ASSERT_INT_WITHIN(5, 12, result);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_median_basic_filtering);
    RUN_TEST(test_median_sequence);
    return UNITY_END();
}

#define TARGET_PLEO 100
#define TARGET_NEXT 200

#if !defined TARGET
    #define TARGET TARGET_PLEO
#endif
#define PLEO_1_0 100
#define PLEO_1_1 110

#if !defined PLEO_API
    #define PLEO_API PLEO_1_1
#endif


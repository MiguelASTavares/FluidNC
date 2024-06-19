#pragma once
#pragma pack(push, 1)

namespace TMC2300_n {

    struct CHOPCONF_t {
        constexpr static uint8_t address = 0x6C;
        union {
            uint32_t sr;
            struct {
                bool enable_drv : 1;  // 0
                uint16_t : 14;
                uint8_t tbl : 2,  // 15
                    : 7,
                    mres : 4;     // 24
                bool intpol : 1,  // 28
                    dedge : 1,    // 29
                    diss2g : 1,   // 30
                    diss2vs : 1;  // 31
            };
        };
    };
    struct GCONF_t {
        constexpr static uint8_t address = 0x00;
        union {
            uint8_t sr;
            struct {
                bool : 1,                // 0
                    extcap : 1,          // 1
                    : 1,                 // 2
                    shaft : 1,           // 3
                    diag_index : 1,      // 4
                    diag_step : 1,       // 5
                    multistep_filt : 1;  // 6
            };
        };
    };

    struct IOIN_t {
        constexpr static uint8_t address = 0x06;
        union {
            uint32_t sr;
            struct {
                bool enn : 1,       //0
                    nstdby : 1,     //1
                    ad0 : 1,        //2
                    ad1 : 1,        //3
                    diag : 1,       //4
                    stepper : 1,    //5
                    pdn_uart : 1,   //6
                    step : 1,       //7
                    spread_en : 1,  //8
                    dir : 1,        //9
                    comp_a1a2 : 1,  //10
                    comp_b1b2 : 1;  //11
                uint16_t : 12;
                uint8_t version : 8;
            };
        };
    };

    //struct GSTAT_t {
    //    constexpr static uint8_t address = 0x01;
    //    union {
    //        uint8_t sr;
    //        struct {
    //            bool reset : 1,   // 0
    //                drv_err : 1,  // 1
    //                u3v5 : 1;     // 2
    //        };
    //    };
    //};

    struct SGTHRS_t {
        constexpr static uint8_t address = 0x40;
        uint8_t                  sr : 8;
    };

    struct SG_RESULT_t {
        constexpr static uint8_t address = 0x41;
        uint16_t                 sr : 10;
    };

    struct COOLCONF_t {
        constexpr static uint8_t address = 0x42;
        union {
            uint16_t sr;
            struct {
                uint8_t semin : 4, : 1, seup : 2, : 1, semax : 4, : 1, sedn : 2;
                bool    seimin : 1;
            };
        };
    };

    struct DRV_STATUS_t {
        constexpr static uint8_t address = 0x6F;
        union {
            uint32_t sr;
            struct {
                bool otpw : 1,      // 0
                    ot : 1,         // 1
                    s2ga : 1,       // 2
                    s2gb : 1,       // 3
                    s2vsa : 1,      // 4
                    s2vsb : 1,      // 5
                    ola : 1,        // 6
                    olb : 1,        // 7
                    t120 : 1,       // 8
                    t150 : 1;       // 9
                uint8_t             //
                    : 6,            //
                    cs_actual : 5;  // 16
                uint16_t : 10;      //
                bool stst : 1;      // 31
            };
        };
    };

    struct TCOOLTHRS_t {
        constexpr static uint8_t address = 0x14;
        uint32_t                 sr : 10;
    };

}

#pragma pack(pop)

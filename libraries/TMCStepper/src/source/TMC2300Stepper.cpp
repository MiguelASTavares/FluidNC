#include "TMCStepper.h"

void TMC2300Stepper::defaults() {
    GCONF(1 << 6);
    GSTAT(0);
    SLAVECONF(0);
    IHOLD_IRUN((8) | (31 << 8) | (1ul << 16));
    TPOWERDOWN(20);
    VACTUAL(0);
    TCOOLTHRS(0);
    SGTHRS(0);
    COOLCONF(0);
    CHOPCONF(0x13008001u);
    PWMCONF(0xC40D1024u);
}

uint32_t TMC2300Stepper::IOIN() {
    return read(TMC2300_n::IOIN_t::address);
}
bool TMC2300Stepper::enn() {
    TMC2300_n::IOIN_t r { 0 };
    r.sr = IOIN();
    return r.enn;
}
bool TMC2300Stepper::nstdby() {
    TMC2300_n::IOIN_t r { 0 };
    r.sr = IOIN();
    return r.nstdby;
}
bool TMC2300Stepper::ms1() {
    TMC2300_n::IOIN_t r { 0 };
    r.sr = IOIN();
    return r.ad0;
}
bool TMC2300Stepper::ms2() {
    TMC2300_n::IOIN_t r { 0 };
    r.sr = IOIN();
    return r.ad1;
}
bool TMC2300Stepper::diag() {
    TMC2300_n::IOIN_t r { 0 };
    r.sr = IOIN();
    return r.diag;
}
bool TMC2300Stepper::pdn_uart() {
    TMC2300_n::IOIN_t r { 0 };
    r.sr = IOIN();
    return r.pdn_uart;
}
bool TMC2300Stepper::step() {
    TMC2300_n::IOIN_t r { 0 };
    r.sr = IOIN();
    return r.step;
}
bool TMC2300Stepper::spread_en() {
    TMC2300_n::IOIN_t r { 0 };
    r.sr = IOIN();
    return r.spread_en;
}
bool TMC2300Stepper::dir() {
    TMC2300_n::IOIN_t r { 0 };
    r.sr = IOIN();
    return r.dir;
}
bool TMC2300Stepper::comp_a1a2() {
    TMC2300_n::IOIN_t r { 0 };
    r.sr = IOIN();
    return r.comp_a1a2;
}
bool TMC2300Stepper::comp_b1b2() {
    TMC2300_n::IOIN_t r { 0 };
    r.sr = IOIN();
    return r.comp_a1a2;
}
uint8_t TMC2300Stepper::version() {
    TMC2300_n::IOIN_t r { 0 };
    r.sr = IOIN();
    return r.version;
}

void TMC2300Stepper::push() {
    IHOLD_IRUN(IHOLD_IRUN_register.sr);
    TPOWERDOWN(TPOWERDOWN_register.sr);
    TPWMTHRS(TPWMTHRS_register.sr);
    GCONF(GCONF_register.sr);
    SLAVECONF(SLAVECONF_register.sr);
    VACTUAL(VACTUAL_register.sr);
    CHOPCONF(CHOPCONF_register.sr);
    PWMCONF(PWMCONF_register.sr);
    TCOOLTHRS(TCOOLTHRS_register.sr);
    SGTHRS(SGTHRS_register.sr);
    COOLCONF(COOLCONF_register.sr);
}

void TMC2300Stepper::SGTHRS(uint8_t input) {
    SGTHRS_register.sr = input;
    write(SGTHRS_register.address, SGTHRS_register.sr);
}
uint8_t TMC2300Stepper::SGTHRS() {
    return SGTHRS_register.sr;
}

// W: TCOOLTHRS
uint32_t TMC2300Stepper::TCOOLTHRS() {
    return TCOOLTHRS_register.sr;
}
void TMC2300Stepper::TCOOLTHRS(uint32_t input) {
    TCOOLTHRS_register.sr = input;
    write(TCOOLTHRS_register.address, TCOOLTHRS_register.sr);
}

uint16_t TMC2300Stepper::SG_RESULT() {
    return read(TMC2300_n::SG_RESULT_t::address);
}

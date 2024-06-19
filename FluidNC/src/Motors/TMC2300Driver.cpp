// Copyright (c) 2020 -	Bart Dring
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

/*
    This is used for Trinamic SPI controlled stepper motor drivers.
*/

#include "TMC2300Driver.h"
#include "../Machine/MachineConfig.h"
#include <atomic>
#include <sstream>  // Include for converting custom types to strings

namespace MotorDrivers {

    // Helper function to convert pointer to string
    template <typename T>
    std::string ptr_to_string(T* ptr) {
        std::ostringstream oss;
        oss << ptr;
        return oss.str();
    }

    void TMC2300Driver::init() {
        log_info("Initializing TMC2300Driver...");
        TrinamicUartDriver::init();
        if (!_uart) {
            log_error("UART not available. Initialization aborted.");
            return;
        }

        if (_r_sense == 0) {
            _r_sense = TMC2300_RSENSE_DEFAULT;
            log_info("R_sense set to default value: " << _r_sense);
        }

        tmc2300 = new TMC2300Stepper(_uart, _r_sense, _addr);
        log_info("TMC2300Stepper object created with UART: " << ptr_to_string(_uart) << ", R_sense: " << _r_sense
                                                             << ", Addr: " << static_cast<int>(_addr));

        registration();
        log_info("TMC2300Driver initialized and registered.");
    }

    void TMC2300Driver::config_motor() {
        log_info("Configuring motor...");
        _cs_pin.synchronousWrite(true);
        //log_info("CS pin set to true.");

        tmc2300->begin();
        log_info("TMC2300Stepper begin() called.");

        TrinamicBase::config_motor();
        log_info("Base motor configuration completed.");

        _cs_pin.synchronousWrite(false);
        //log_info("CS pin set to false.");
    }

    void TMC2300Driver::set_registers(bool isHoming) {
        log_info("Setting registers with isHoming: " << isHoming);
        if (_has_errors) {
            log_error("Errors detected, aborting set_registers.");
            return;
        }

        TrinamicMode _mode = static_cast<TrinamicMode>(trinamicModes[isHoming ? _homing_mode : _run_mode].value);
        log_info("Selected mode: " << static_cast<int>(_mode));

        float    _mode_current = isHoming ? _homing_current : _run_current;
        uint16_t run_i         = (uint16_t)(_mode_current * 1000.0);
        log_info("Run current (mA): " << run_i);

        _cs_pin.synchronousWrite(true);
        //log_info("CS pin set to true.");

        tmc2300->I_scale_analog(false);
        tmc2300->rms_current(run_i, TrinamicBase::holdPercent());
        log_info("RMS current and hold percentage set.");

        int usteps = _microsteps == 1 ? 0 : _microsteps;
        tmc2300->microsteps(usteps);
        log_info("Microsteps set to: " << usteps);

        tmc2300->pdn_disable(true);
        log_info("Powerdown pin disabled.");

        switch (_mode) {
            case TrinamicMode::StealthChop:
                log_info(axisName() << " StealthChop mode selected.");
                tmc2300->en_spreadCycle(false);
                tmc2300->pwm_autoscale(true);
                log_info("StealthChop configuration applied.");
                break;
            case TrinamicMode::CoolStep:
                log_info(axisName() << " CoolStep mode selected.");
                tmc2300->en_spreadCycle(true);
                tmc2300->pwm_autoscale(false);
                log_info("CoolStep configuration applied.");
                break;
            case TrinamicMode::StallGuard: {
                log_info(axisName() << " StallGuard mode selected.");
                auto axisConfig     = config->_axes->_axis[this->axis_index()];
                auto homingFeedRate = (axisConfig->_homing != nullptr) ? axisConfig->_homing->_feedRate : 200;
                log_info("Homing feed rate: " << homingFeedRate);

                tmc2300->pwm_autoscale(true);
                tmc2300->TCOOLTHRS(calc_tstep(homingFeedRate, 150.0));
                tmc2300->SGTHRS(_stallguard);
                log_info("StallGuard configuration applied.");
                break;
            }
        }

        log_info("Register dump:");
        log_info("CHOPCONF: " << to_hex(tmc2300->CHOPCONF()));
        log_info("COOLCONF: " << to_hex(tmc2300->COOLCONF()));
        log_info("TPWMTHRS: " << to_hex(tmc2300->TPWMTHRS()));
        log_info("TCOOLTHRS: " << to_hex(tmc2300->TCOOLTHRS()));
        log_info("GCONF: " << to_hex(tmc2300->GCONF()));
        log_info("PWMCONF: " << to_hex(tmc2300->PWMCONF()));
        log_info("IHOLD_IRUN: " << to_hex(tmc2300->IHOLD_IRUN()));

        _cs_pin.synchronousWrite(false);
        //log_info("CS pin set to false.");
    }

    void TMC2300Driver::debug_message() {
        log_info("Generating debug message...");
        if (_has_errors) {
            log_error("Errors detected, aborting debug_message.");
            return;
        }

        _cs_pin.synchronousWrite(true);
        //log_info("CS pin set to true.");

        uint32_t tstep = tmc2300->TSTEP();
        log_info("TSTEP value: " << tstep);

        if (tstep == 0xFFFFF || tstep < 1) {  // if axis is not moving return
            log_info("Axis not moving, tstep indicates idle.");
            _cs_pin.synchronousWrite(false);
            //log_info("CS pin set to false.");
            return;
        }
        float feedrate = Stepper::get_realtime_rate();  //* settings.microsteps[axis_index] / 60.0 ; // convert mm/min to Hz
        log_info("Realtime feedrate: " << feedrate << " mm/min");

        if (tmc2300) {
            log_info(axisName() << " SG_Val: " << tmc2300->SG_RESULT() << " Rate: " << feedrate << " mm/min SG_Setting: " << _stallguard);
        }

        _cs_pin.synchronousWrite(false);
        //log_info("CS pin set to false.");
    }

    void TMC2300Driver::set_disable(bool disable) {
        log_info("Setting disable to: " << disable);
        _cs_pin.synchronousWrite(true);
        //log_info("CS pin set to true.");

        if (TrinamicUartDriver::startDisable(disable)) {
            log_info("StartDisable called with disable: " << disable);
            if (_use_enable) {
                tmc2300->toff(TrinamicUartDriver::toffValue());
                log_info("Driver toff value set.");
            }
        }

        _cs_pin.synchronousWrite(false);
        //log_info("CS pin set to false.");
    }

    bool TMC2300Driver::test() {
        log_info("Testing TMC2300Driver version...");
        _cs_pin.synchronousWrite(true);
        //log_info("CS pin set to true.");

        if (!checkVersion(0x40, tmc2300->version())) {
            log_error("Version check failed.");
            _cs_pin.synchronousWrite(false);
            //log_info("CS pin set to false.");
            return false;
        }
        log_info("Version check passed.");

        uint8_t ifcnt_before = tmc2300->IFCNT();
        log_info("IFCNT before: " << static_cast<int>(ifcnt_before));

        tmc2300->GSTAT(0);  // clear GSTAT to increase ifcnt
        uint8_t ifcnt_after = tmc2300->IFCNT();
        log_info("IFCNT after: " << static_cast<int>(ifcnt_after));

        bool okay = ((ifcnt_before + 1) & 0xff) == ifcnt_after;
        if (!okay) {
            log_error("Communication failure detected.");
            TrinamicBase::reportCommsFailure();
            _cs_pin.synchronousWrite(false);
            //log_info("CS pin set to false.");
            return false;
        }
        log_info("Communication test passed.");

        _cs_pin.synchronousWrite(false);
        //log_info("CS pin set to false.");
        return true;
    }

    // Configuration registration
    namespace {
        MotorFactory::InstanceBuilder<TMC2300Driver> registration("tmc_2300");
    }
}

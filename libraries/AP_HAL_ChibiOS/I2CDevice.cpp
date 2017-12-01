/*
 * This file is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "I2CDevice.h"

#include <AP_HAL/AP_HAL.h>

#include "Util.h"
#include "Scheduler.h"

#include "ch.h"
#include "hal.h"

static I2CDriver* I2CD[] = {
    &I2CD2,
    &I2CD1
};
namespace ChibiOS {

DeviceBus I2CDevice::businfo[I2CDevice::num_buses];

I2CDevice::I2CDevice(uint8_t bus, uint8_t address) :
    _retries(2),
    _busnum(bus),
    _address(address)
{
    set_device_bus(bus);
    set_device_address(address);
    asprintf(&pname, "I2C:%u:%02x",
             (unsigned)bus, (unsigned)address);
}
    
I2CDevice::~I2CDevice()
{
    printf("I2C device bus %u address 0x%02x closed\n", 
           (unsigned)_busnum, (unsigned)_address);
    free(pname);
}

bool I2CDevice::transfer(const uint8_t *send, uint32_t send_len,
                         uint8_t *recv, uint32_t recv_len)
{
    if (!init_done) {
        i2ccfg.op_mode = OPMODE_I2C;
        i2ccfg.clock_speed = 400000;
        i2ccfg.duty_cycle = FAST_DUTY_CYCLE_2;
        i2cStart(I2CD[_busnum], &i2ccfg);
        init_done = true;
    }
    if (_split_transfers) {
        /*
          splitting the transfer() into two pieces avoids a stop condition
          with SCL low which is not supported on some devices (such as
          LidarLite blue label)
        */
        if (send && send_len) {
            if (!_transfer(send, send_len, nullptr, 0)) {
                return false;
            }
        }
        if (recv && recv_len) {
            if (!_transfer(nullptr, 0, recv, recv_len)) {
                return false;
            }
        }
    } else {
        // combined transfer
        if (!_transfer(send, send_len, recv, recv_len)) {
            return false;
        }
    }
    return true;
}

bool I2CDevice::_transfer(const uint8_t *send, uint32_t send_len,
                         uint8_t *recv, uint32_t recv_len)
{
    int ret;
    for(uint8_t i=0 ; i <= _retries; i++) {
        i2cAcquireBus(I2CD[_busnum]);
        if(send_len == 0) {
            ret = i2cMasterReceiveTimeout(I2CD[_busnum], _address,recv, recv_len, MS2ST(4));
        
        } else {
            ret = i2cMasterTransmitTimeout(I2CD[_busnum], _address, send, send_len,
                                     recv, recv_len, MS2ST(4));
        }
        i2cReleaseBus(I2CD[_busnum]);
        if (ret != MSG_OK){
            _errors = i2cGetErrors(I2CD[_busnum]);
            //restart the bus
            i2cStop(I2CD[_busnum]);
            i2cStart(I2CD[_busnum], &i2ccfg);
        } else {
            return true;
        }
    }
    return false;
}

bool I2CDevice::read_registers_multiple(uint8_t first_reg, uint8_t *recv,
                                        uint32_t recv_len, uint8_t times)
{
    return false;
}

    
/*
  register a periodic callback
*/
AP_HAL::Device::PeriodicHandle I2CDevice::register_periodic_callback(uint32_t period_usec, AP_HAL::Device::PeriodicCb cb)
{
    if (_busnum >= num_buses) {
        return nullptr;
    }
    struct DeviceBus &binfo = businfo[_busnum];
    return binfo.register_periodic_callback(period_usec, cb, this);
}
    

/*
  adjust a periodic callback
*/
bool I2CDevice::adjust_periodic_callback(AP_HAL::Device::PeriodicHandle h, uint32_t period_usec)
{
    if (_busnum >= num_buses) {
        return false;
    }

    struct DeviceBus &binfo = businfo[_busnum];

    return binfo.adjust_timer(h, period_usec);
}

AP_HAL::OwnPtr<AP_HAL::I2CDevice>
I2CDeviceManager::get_device(uint8_t bus, uint8_t address)
{
    auto dev = AP_HAL::OwnPtr<AP_HAL::I2CDevice>(new I2CDevice(bus, address));
    return dev;
}

}
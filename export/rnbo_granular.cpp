/*******************************************************************************************************************
Copyright (c) 2023 Cycling '74

The code that Max generates automatically and that end users are capable of
exporting and using, and any associated documentation files (the “Software”)
is a work of authorship for which Cycling '74 is the author and owner for
copyright purposes.

This Software is dual-licensed either under the terms of the Cycling '74
License for Max-Generated Code for Export, or alternatively under the terms
of the General Public License (GPL) Version 3. You may use the Software
according to either of these licenses as it is most appropriate for your
project on a case-by-case basis (proprietary or not).

A) Cycling '74 License for Max-Generated Code for Export

A license is hereby granted, free of charge, to any person obtaining a copy
of the Software (“Licensee”) to use, copy, modify, merge, publish, and
distribute copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The Software is licensed to Licensee for all uses that do not include the sale,
sublicensing, or commercial distribution of software that incorporates this
source code. This means that the Licensee is free to use this software for
educational, research, and prototyping purposes, to create musical or other
creative works with software that incorporates this source code, or any other
use that does not constitute selling software that makes use of this source
code. Commercial distribution also includes the packaging of free software with
other paid software, hardware, or software-provided commercial services.

For entities with UNDER $200k in annual revenue or funding, a license is hereby
granted, free of charge, for the sale, sublicensing, or commercial distribution
of software that incorporates this source code, for as long as the entity's
annual revenue remains below $200k annual revenue or funding.

For entities with OVER $200k in annual revenue or funding interested in the
sale, sublicensing, or commercial distribution of software that incorporates
this source code, please send inquiries to licensing@cycling74.com.

The above copyright notice and this license shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Please see
https://support.cycling74.com/hc/en-us/articles/10730637742483-RNBO-Export-Licensing-FAQ
for additional information

B) General Public License Version 3 (GPLv3)
Details of the GPLv3 license can be found at: https://www.gnu.org/licenses/gpl-3.0.html
*******************************************************************************************************************/

#ifdef RNBO_LIB_PREFIX
#define STR_IMPL(A) #A
#define STR(A) STR_IMPL(A)
#define RNBO_LIB_INCLUDE(X) STR(RNBO_LIB_PREFIX/X)
#else
#define RNBO_LIB_INCLUDE(X) #X
#endif // RNBO_LIB_PREFIX
#ifdef RNBO_INJECTPLATFORM
#define RNBO_USECUSTOMPLATFORM
#include RNBO_INJECTPLATFORM
#endif // RNBO_INJECTPLATFORM

#include RNBO_LIB_INCLUDE(RNBO_Common.h)
#include RNBO_LIB_INCLUDE(RNBO_AudioSignal.h)

namespace RNBO {


#define trunc(x) ((Int)(x))
#define autoref auto&

#if defined(__GNUC__) || defined(__clang__)
    #define RNBO_RESTRICT __restrict__
#elif defined(_MSC_VER)
    #define RNBO_RESTRICT __restrict
#endif

#define FIXEDSIZEARRAYINIT(...) { }

template <class ENGINE = INTERNALENGINE> class RnboGranular : public PatcherInterfaceImpl {

friend class EngineCore;
friend class Engine;
friend class MinimalEngine<>;
public:

RnboGranular()
: _internalEngine(this)
{
}

~RnboGranular()
{
    deallocateSignals();
}

Index getNumMidiInputPorts() const {
    return 1;
}

void processMidiEvent(MillisecondTime time, int port, ConstByteArray data, Index length) {
    this->updateTime(time, (ENGINE*)nullptr);
    this->ctlin_01_midihandler(data[0] & 240, (data[0] & 15) + 1, port, data, length);
    this->ctlin_02_midihandler(data[0] & 240, (data[0] & 15) + 1, port, data, length);
    this->ctlin_03_midihandler(data[0] & 240, (data[0] & 15) + 1, port, data, length);
    this->ctlin_04_midihandler(data[0] & 240, (data[0] & 15) + 1, port, data, length);
    this->ctlin_05_midihandler(data[0] & 240, (data[0] & 15) + 1, port, data, length);
    this->ctlin_06_midihandler(data[0] & 240, (data[0] & 15) + 1, port, data, length);
    this->ctlin_07_midihandler(data[0] & 240, (data[0] & 15) + 1, port, data, length);
}

Index getNumMidiOutputPorts() const {
    return 0;
}

void process(
    const SampleValue * const* inputs,
    Index numInputs,
    SampleValue * const* outputs,
    Index numOutputs,
    Index n
) {
    this->vs = n;
    this->updateTime(this->getEngine()->getCurrentTime(), (ENGINE*)nullptr, true);
    SampleValue * out1 = (numOutputs >= 1 && outputs[0] ? outputs[0] : this->dummyBuffer);
    SampleValue * out2 = (numOutputs >= 2 && outputs[1] ? outputs[1] : this->dummyBuffer);
    const SampleValue * in1 = (numInputs >= 1 && inputs[0] ? inputs[0] : this->zeroBuffer);
    const SampleValue * in2 = (numInputs >= 2 && inputs[1] ? inputs[1] : this->zeroBuffer);

    this->gen_01_perform(
        in1,
        in2,
        this->gen_01_interval,
        this->gen_01_grainSize,
        this->gen_01_grainPos,
        this->gen_01_width,
        this->gen_01_pitch,
        this->gen_01_mix,
        this->gen_01_gain,
        out1,
        out2,
        n
    );

    this->stackprotect_perform(n);
    this->globaltransport_advance();
    this->advanceTime((ENGINE*)nullptr);
    this->audioProcessSampleCount += this->vs;
}

void prepareToProcess(number sampleRate, Index maxBlockSize, bool force) {
    RNBO_ASSERT(this->_isInitialized);

    if (this->maxvs < maxBlockSize || !this->didAllocateSignals) {
        this->globaltransport_tempo = resizeSignal(this->globaltransport_tempo, this->maxvs, maxBlockSize);
        this->globaltransport_state = resizeSignal(this->globaltransport_state, this->maxvs, maxBlockSize);
        this->zeroBuffer = resizeSignal(this->zeroBuffer, this->maxvs, maxBlockSize);
        this->dummyBuffer = resizeSignal(this->dummyBuffer, this->maxvs, maxBlockSize);
        this->didAllocateSignals = true;
    }

    const bool sampleRateChanged = sampleRate != this->sr;
    const bool maxvsChanged = maxBlockSize != this->maxvs;
    const bool forceDSPSetup = sampleRateChanged || maxvsChanged || force;

    if (sampleRateChanged || maxvsChanged) {
        this->vs = maxBlockSize;
        this->maxvs = maxBlockSize;
        this->sr = sampleRate;
        this->invsr = 1 / sampleRate;
    }

    this->gen_01_dspsetup(forceDSPSetup);
    this->globaltransport_dspsetup(forceDSPSetup);

    if (sampleRateChanged)
        this->onSampleRateChanged(sampleRate);
}

number msToSamps(MillisecondTime ms, number sampleRate) {
    return ms * sampleRate * 0.001;
}

MillisecondTime sampsToMs(SampleIndex samps) {
    return samps * (this->invsr * 1000);
}

Index getNumInputChannels() const {
    return 2;
}

Index getNumOutputChannels() const {
    return 2;
}

DataRef* getDataRef(DataRefIndex index)  {
    switch (index) {
    case 0:
        {
        return addressOf(this->data_param);
        break;
        }
    case 1:
        {
        return addressOf(this->gen_01_del_inL_bufferobj);
        break;
        }
    case 2:
        {
        return addressOf(this->gen_01_del_inR_bufferobj);
        break;
        }
    default:
        {
        return nullptr;
        }
    }
}

DataRefIndex getNumDataRefs() const {
    return 3;
}

void processDataViewUpdate(DataRefIndex index, MillisecondTime time) {
    this->updateTime(time, (ENGINE*)nullptr);

    if (index == 0) {
        this->gen_01_data_param = reInitDataView(this->gen_01_data_param, this->data_param);
    }

    if (index == 1) {
        this->gen_01_del_inL_buffer = reInitDataView(this->gen_01_del_inL_buffer, this->gen_01_del_inL_bufferobj);
    }

    if (index == 2) {
        this->gen_01_del_inR_buffer = reInitDataView(this->gen_01_del_inR_buffer, this->gen_01_del_inR_bufferobj);
    }
}

void initialize() {
    RNBO_ASSERT(!this->_isInitialized);

    this->data_param = initDataRef(
        this->data_param,
        this->dataRefStrings->name0,
        true,
        this->dataRefStrings->file0,
        this->dataRefStrings->tag0
    );

    this->gen_01_del_inL_bufferobj = initDataRef(
        this->gen_01_del_inL_bufferobj,
        this->dataRefStrings->name1,
        true,
        this->dataRefStrings->file1,
        this->dataRefStrings->tag1
    );

    this->gen_01_del_inR_bufferobj = initDataRef(
        this->gen_01_del_inR_bufferobj,
        this->dataRefStrings->name2,
        true,
        this->dataRefStrings->file2,
        this->dataRefStrings->tag2
    );

    this->assign_defaults();
    this->applyState();
    this->data_param->setIndex(0);
    this->gen_01_data_param = new SampleBuffer(this->data_param);
    this->gen_01_del_inL_bufferobj->setIndex(1);
    this->gen_01_del_inL_buffer = new Float64Buffer(this->gen_01_del_inL_bufferobj);
    this->gen_01_del_inR_bufferobj->setIndex(2);
    this->gen_01_del_inR_buffer = new Float64Buffer(this->gen_01_del_inR_bufferobj);
    this->initializeObjects();
    this->allocateDataRefs();
    this->startup();
    this->_isInitialized = true;
}

void getPreset(PatcherStateInterface& preset) {
    this->updateTime(this->getEngine()->getCurrentTime(), (ENGINE*)nullptr);
    preset["__presetid"] = "rnbo";
    this->param_01_getPresetValue(getSubState(preset, "width"));
    this->param_02_getPresetValue(getSubState(preset, "gain"));
    this->param_03_getPresetValue(getSubState(preset, "mix"));
    this->param_04_getPresetValue(getSubState(preset, "pitch"));
    this->param_05_getPresetValue(getSubState(preset, "grainPos"));
    this->param_06_getPresetValue(getSubState(preset, "grainSize"));
    this->param_07_getPresetValue(getSubState(preset, "interval"));
}

void setPreset(MillisecondTime time, PatcherStateInterface& preset) {
    this->updateTime(time, (ENGINE*)nullptr);
    this->param_01_setPresetValue(getSubState(preset, "width"));
    this->param_02_setPresetValue(getSubState(preset, "gain"));
    this->param_03_setPresetValue(getSubState(preset, "mix"));
    this->param_04_setPresetValue(getSubState(preset, "pitch"));
    this->param_05_setPresetValue(getSubState(preset, "grainPos"));
    this->param_06_setPresetValue(getSubState(preset, "grainSize"));
    this->param_07_setPresetValue(getSubState(preset, "interval"));
}

void setParameterValue(ParameterIndex index, ParameterValue v, MillisecondTime time) {
    this->updateTime(time, (ENGINE*)nullptr);

    switch (index) {
    case 0:
        {
        this->param_01_value_set(v);
        break;
        }
    case 1:
        {
        this->param_02_value_set(v);
        break;
        }
    case 2:
        {
        this->param_03_value_set(v);
        break;
        }
    case 3:
        {
        this->param_04_value_set(v);
        break;
        }
    case 4:
        {
        this->param_05_value_set(v);
        break;
        }
    case 5:
        {
        this->param_06_value_set(v);
        break;
        }
    case 6:
        {
        this->param_07_value_set(v);
        break;
        }
    }
}

void processParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValue(index, value, time);
}

void processParameterBangEvent(ParameterIndex index, MillisecondTime time) {
    this->setParameterValue(index, this->getParameterValue(index), time);
}

void processNormalizedParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValueNormalized(index, value, time);
}

ParameterValue getParameterValue(ParameterIndex index)  {
    switch (index) {
    case 0:
        {
        return this->param_01_value;
        }
    case 1:
        {
        return this->param_02_value;
        }
    case 2:
        {
        return this->param_03_value;
        }
    case 3:
        {
        return this->param_04_value;
        }
    case 4:
        {
        return this->param_05_value;
        }
    case 5:
        {
        return this->param_06_value;
        }
    case 6:
        {
        return this->param_07_value;
        }
    default:
        {
        return 0;
        }
    }
}

ParameterIndex getNumSignalInParameters() const {
    return 0;
}

ParameterIndex getNumSignalOutParameters() const {
    return 0;
}

ParameterIndex getNumParameters() const {
    return 7;
}

ConstCharPointer getParameterName(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "width";
        }
    case 1:
        {
        return "gain";
        }
    case 2:
        {
        return "mix";
        }
    case 3:
        {
        return "pitch";
        }
    case 4:
        {
        return "grainPos";
        }
    case 5:
        {
        return "grainSize";
        }
    case 6:
        {
        return "interval";
        }
    default:
        {
        return "bogus";
        }
    }
}

ConstCharPointer getParameterId(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "width";
        }
    case 1:
        {
        return "gain";
        }
    case 2:
        {
        return "mix";
        }
    case 3:
        {
        return "pitch";
        }
    case 4:
        {
        return "grainPos";
        }
    case 5:
        {
        return "grainSize";
        }
    case 6:
        {
        return "interval";
        }
    default:
        {
        return "bogus";
        }
    }
}

void getParameterInfo(ParameterIndex index, ParameterInfo * info) const {
    {
        switch (index) {
        case 0:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 50;
            info->min = 0;
            info->max = 100;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 1:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = -36;
            info->max = 12;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 2:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 50;
            info->min = 0;
            info->max = 100;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 3:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = -12;
            info->max = 12;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 4:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 100;
            info->min = 10;
            info->max = 500;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 5:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 100;
            info->min = 10;
            info->max = 500;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 6:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 100;
            info->min = 10;
            info->max = 500;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        }
    }
}

ParameterValue applyStepsToNormalizedParameterValue(ParameterValue normalizedValue, int steps) const {
    if (steps == 1) {
        if (normalizedValue > 0) {
            normalizedValue = 1.;
        }
    } else {
        ParameterValue oneStep = (number)1. / (steps - 1);
        ParameterValue numberOfSteps = rnbo_fround(normalizedValue / oneStep * 1 / (number)1) * (number)1;
        normalizedValue = numberOfSteps * oneStep;
    }

    return normalizedValue;
}

ParameterValue convertToNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 0:
    case 2:
        {
        {
            value = (value < 0 ? 0 : (value > 100 ? 100 : value));
            ParameterValue normalizedValue = (value - 0) / (100 - 0);
            return normalizedValue;
        }
        }
    case 4:
    case 5:
    case 6:
        {
        {
            value = (value < 10 ? 10 : (value > 500 ? 500 : value));
            ParameterValue normalizedValue = (value - 10) / (500 - 10);
            return normalizedValue;
        }
        }
    case 1:
        {
        {
            value = (value < -36 ? -36 : (value > 12 ? 12 : value));
            ParameterValue normalizedValue = (value - -36) / (12 - -36);
            return normalizedValue;
        }
        }
    case 3:
        {
        {
            value = (value < -12 ? -12 : (value > 12 ? 12 : value));
            ParameterValue normalizedValue = (value - -12) / (12 - -12);
            return normalizedValue;
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue convertFromNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    value = (value < 0 ? 0 : (value > 1 ? 1 : value));

    switch (index) {
    case 0:
    case 2:
        {
        {
            {
                return 0 + value * (100 - 0);
            }
        }
        }
    case 4:
    case 5:
    case 6:
        {
        {
            {
                return 10 + value * (500 - 10);
            }
        }
        }
    case 1:
        {
        {
            {
                return -36 + value * (12 - -36);
            }
        }
        }
    case 3:
        {
        {
            {
                return -12 + value * (12 - -12);
            }
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue constrainParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 0:
        {
        return this->param_01_value_constrain(value);
        }
    case 1:
        {
        return this->param_02_value_constrain(value);
        }
    case 2:
        {
        return this->param_03_value_constrain(value);
        }
    case 3:
        {
        return this->param_04_value_constrain(value);
        }
    case 4:
        {
        return this->param_05_value_constrain(value);
        }
    case 5:
        {
        return this->param_06_value_constrain(value);
        }
    case 6:
        {
        return this->param_07_value_constrain(value);
        }
    default:
        {
        return value;
        }
    }
}

void processNumMessage(MessageTag , MessageTag , MillisecondTime , number ) {}

void processListMessage(MessageTag , MessageTag , MillisecondTime , const list& ) {}

void processBangMessage(MessageTag , MessageTag , MillisecondTime ) {}

MessageTagInfo resolveTag(MessageTag tag) const {
    switch (tag) {

    }

    return "";
}

MessageIndex getNumMessages() const {
    return 0;
}

const MessageInfo& getMessageInfo(MessageIndex index) const {
    switch (index) {

    }

    return NullMessageInfo;
}

protected:

		
void advanceTime(EXTERNALENGINE*) {}
void advanceTime(INTERNALENGINE*) {
	_internalEngine.advanceTime(sampstoms(this->vs));
}

void processInternalEvents(MillisecondTime time) {
	_internalEngine.processEventsUntil(time);
}

void updateTime(MillisecondTime time, INTERNALENGINE*, bool inProcess = false) {
	if (time == TimeNow) time = getPatcherTime();
	processInternalEvents(inProcess ? time + sampsToMs(this->vs) : time);
	updateTime(time, (EXTERNALENGINE*)nullptr);
}

RnboGranular* operator->() {
    return this;
}
const RnboGranular* operator->() const {
    return this;
}
RnboGranular* getTopLevelPatcher() {
    return this;
}

void cancelClockEvents()
{
}

template<typename LISTTYPE = list> void listquicksort(LISTTYPE& arr, LISTTYPE& sortindices, Int l, Int h, bool ascending) {
    if (l < h) {
        Int p = (Int)(this->listpartition(arr, sortindices, l, h, ascending));
        this->listquicksort(arr, sortindices, l, p - 1, ascending);
        this->listquicksort(arr, sortindices, p + 1, h, ascending);
    }
}

template<typename LISTTYPE = list> Int listpartition(LISTTYPE& arr, LISTTYPE& sortindices, Int l, Int h, bool ascending) {
    number x = arr[(Index)h];
    Int i = (Int)(l - 1);

    for (Int j = (Int)(l); j <= h - 1; j++) {
        bool asc = (bool)((bool)(ascending) && arr[(Index)j] <= x);
        bool desc = (bool)((bool)(!(bool)(ascending)) && arr[(Index)j] >= x);

        if ((bool)(asc) || (bool)(desc)) {
            i++;
            this->listswapelements(arr, i, j);
            this->listswapelements(sortindices, i, j);
        }
    }

    i++;
    this->listswapelements(arr, i, h);
    this->listswapelements(sortindices, i, h);
    return i;
}

template<typename LISTTYPE = list> void listswapelements(LISTTYPE& arr, Int a, Int b) {
    auto tmp = arr[(Index)a];
    arr[(Index)a] = arr[(Index)b];
    arr[(Index)b] = tmp;
}

inline number linearinterp(number frac, number x, number y) {
    return x + (y - x) * frac;
}

inline number cubicinterp(number a, number w, number x, number y, number z) {
    number a1 = 1. + a;
    number aa = a * a1;
    number b = 1. - a;
    number b1 = 2. - a;
    number bb = b * b1;
    number fw = -.1666667 * bb * a;
    number fx = .5 * bb * a1;
    number fy = .5 * aa * b1;
    number fz = -.1666667 * aa * b;
    return w * fw + x * fx + y * fy + z * fz;
}

inline number fastcubicinterp(number a, number w, number x, number y, number z) {
    number a2 = a * a;
    number f0 = z - y - w + x;
    number f1 = w - x - f0;
    number f2 = y - w;
    number f3 = x;
    return f0 * a * a2 + f1 * a2 + f2 * a + f3;
}

inline number splineinterp(number a, number w, number x, number y, number z) {
    number a2 = a * a;
    number f0 = -0.5 * w + 1.5 * x - 1.5 * y + 0.5 * z;
    number f1 = w - 2.5 * x + 2 * y - 0.5 * z;
    number f2 = -0.5 * w + 0.5 * y;
    return f0 * a * a2 + f1 * a2 + f2 * a + x;
}

inline number spline6interp(number a, number y0, number y1, number y2, number y3, number y4, number y5) {
    number ym2py2 = y0 + y4;
    number ym1py1 = y1 + y3;
    number y2mym2 = y4 - y0;
    number y1mym1 = y3 - y1;
    number sixthym1py1 = (number)1 / (number)6.0 * ym1py1;
    number c0 = (number)1 / (number)120.0 * ym2py2 + (number)13 / (number)60.0 * ym1py1 + (number)11 / (number)20.0 * y2;
    number c1 = (number)1 / (number)24.0 * y2mym2 + (number)5 / (number)12.0 * y1mym1;
    number c2 = (number)1 / (number)12.0 * ym2py2 + sixthym1py1 - (number)1 / (number)2.0 * y2;
    number c3 = (number)1 / (number)12.0 * y2mym2 - (number)1 / (number)6.0 * y1mym1;
    number c4 = (number)1 / (number)24.0 * ym2py2 - sixthym1py1 + (number)1 / (number)4.0 * y2;
    number c5 = (number)1 / (number)120.0 * (y5 - y0) + (number)1 / (number)24.0 * (y1 - y4) + (number)1 / (number)12.0 * (y3 - y2);
    return ((((c5 * a + c4) * a + c3) * a + c2) * a + c1) * a + c0;
}

inline number cosT8(number r) {
    number t84 = 56.0;
    number t83 = 1680.0;
    number t82 = 20160.0;
    number t81 = 2.4801587302e-05;
    number t73 = 42.0;
    number t72 = 840.0;
    number t71 = 1.9841269841e-04;

    if (r < 0.785398163397448309615660845819875721 && r > -0.785398163397448309615660845819875721) {
        number rr = r * r;
        return 1.0 - rr * t81 * (t82 - rr * (t83 - rr * (t84 - rr)));
    } else if (r > 0.0) {
        r -= 1.57079632679489661923132169163975144;
        number rr = r * r;
        return -r * (1.0 - t71 * rr * (t72 - rr * (t73 - rr)));
    } else {
        r += 1.57079632679489661923132169163975144;
        number rr = r * r;
        return r * (1.0 - t71 * rr * (t72 - rr * (t73 - rr)));
    }
}

inline number cosineinterp(number frac, number x, number y) {
    number a2 = (1.0 - this->cosT8(frac * 3.14159265358979323846)) / (number)2.0;
    return x * (1.0 - a2) + y * a2;
}

number mstosamps(MillisecondTime ms) {
    return ms * this->sr * 0.001;
}

number maximum(number x, number y) {
    return (x < y ? y : x);
}

Index voice() {
    return this->_voiceIndex;
}

number random(number low, number high) {
    number range = high - low;
    return globalrandom() * range + low;
}

inline number safemod(number f, number m) {
    if (m != 0) {
        Int f_trunc = (Int)(trunc(f));
        Int m_trunc = (Int)(trunc(m));

        if (f == f_trunc && m == m_trunc) {
            f = f_trunc % m_trunc;
        } else {
            if (m < 0) {
                m = -m;
            }

            if (f >= m) {
                if (f >= m * 2.0) {
                    number d = f / m;
                    Int i = (Int)(trunc(d));
                    d = d - i;
                    f = d * m;
                } else {
                    f -= m;
                }
            } else if (f <= -m) {
                if (f <= -m * 2.0) {
                    number d = f / m;
                    Int i = (Int)(trunc(d));
                    d = d - i;
                    f = d * m;
                } else {
                    f += m;
                }
            }
        }
    } else {
        f = 0.0;
    }

    return f;
}

number wrap(number x, number low, number high) {
    number lo;
    number hi;

    if (low == high)
        return low;

    if (low > high) {
        hi = low;
        lo = high;
    } else {
        lo = low;
        hi = high;
    }

    number range = hi - lo;

    if (x >= lo && x < hi)
        return x;

    if (range <= 0.000000001)
        return lo;

    Int numWraps = (Int)(trunc((x - lo) / range));
    numWraps = numWraps - ((x < lo ? 1 : 0));
    number result = x - range * numWraps;

    if (result >= hi)
        return result - range;
    else
        return result;
}

template<typename BUFFERTYPE> array<SampleValue, 1 + 1> peek_default(BUFFERTYPE& buffer, SampleValue sampleIndex, Index channelOffset) {
    number bufferSize = buffer->getSize();
    const Index bufferChannels = (const Index)(buffer->getChannels());
    constexpr int ___N2 = 1 + 1;
    array<SampleValue, ___N2> out = FIXEDSIZEARRAYINIT(1 + 1);

    if (bufferSize == 0 || (5 == 5 && (sampleIndex < 0 || sampleIndex >= bufferSize))) {
        return out;
    } else {
        for (Index channel = 0; channel < 1; channel++) {
            Index channelIndex = (Index)(channel + channelOffset);

            {
                if (channelIndex >= bufferChannels || channelIndex < 0) {
                    out[(Index)channel] = 0;
                    continue;
                }
            }

            SampleValue bufferreadsample_result;

            {
                auto& __buffer = buffer;

                if (sampleIndex < 0.0) {
                    bufferreadsample_result = 0.0;
                }

                SampleIndex truncIndex = (SampleIndex)(trunc(sampleIndex));

                {
                    bufferreadsample_result = __buffer->getSample(channelIndex, truncIndex);
                }
            }

            out[(Index)channel] = bufferreadsample_result;
        }

        out[1] = sampleIndex;
        return out;
    }
}

template<typename BUFFERTYPE> void poke_default(
    BUFFERTYPE& buffer,
    SampleValue value,
    SampleValue sampleIndex,
    Int channel,
    number overdub
) {
    number bufferSize = buffer->getSize();
    const Index bufferChannels = (const Index)(buffer->getChannels());

    if (bufferSize > 0 && (5 != 5 || (sampleIndex >= 0 && sampleIndex < bufferSize)) && (5 != 5 || (channel >= 0 && channel < bufferChannels))) {
        if (overdub != 0) {
            number currentValue = buffer->getSample(channel, sampleIndex);

            {
                value = value + currentValue * overdub;
            }
        }

        buffer->setSample(channel, sampleIndex, value);
        buffer->setTouched(true);
    }
}

number fromnormalized(Index index, number normalizedValue) {
    return this->convertFromNormalizedParameterValue(index, normalizedValue);
}

MillisecondTime sampstoms(number samps) {
    return samps * 1000 / this->sr;
}

void param_01_value_set(number v) {
    v = this->param_01_value_constrain(v);
    this->param_01_value = v;
    this->sendParameter(0, false);

    if (this->param_01_value != this->param_01_lastValue) {
        {
            this->getEngine()->presetTouched();
        }

        this->param_01_lastValue = this->param_01_value;
    }

    this->expr_02_in1_set(v);
}

void param_02_value_set(number v) {
    v = this->param_02_value_constrain(v);
    this->param_02_value = v;
    this->sendParameter(1, false);

    if (this->param_02_value != this->param_02_lastValue) {
        {
            this->getEngine()->presetTouched();
        }

        this->param_02_lastValue = this->param_02_value;
    }

    this->expr_04_in1_set(v);
}

void param_03_value_set(number v) {
    v = this->param_03_value_constrain(v);
    this->param_03_value = v;
    this->sendParameter(2, false);

    if (this->param_03_value != this->param_03_lastValue) {
        {
            this->getEngine()->presetTouched();
        }

        this->param_03_lastValue = this->param_03_value;
    }

    this->expr_01_in1_set(v);
}

void param_04_value_set(number v) {
    v = this->param_04_value_constrain(v);
    this->param_04_value = v;
    this->sendParameter(3, false);

    if (this->param_04_value != this->param_04_lastValue) {
        {
            this->getEngine()->presetTouched();
        }

        this->param_04_lastValue = this->param_04_value;
    }

    this->gen_01_pitch_set(v);
}

void param_05_value_set(number v) {
    v = this->param_05_value_constrain(v);
    this->param_05_value = v;
    this->sendParameter(4, false);

    if (this->param_05_value != this->param_05_lastValue) {
        {
            this->getEngine()->presetTouched();
        }

        this->param_05_lastValue = this->param_05_value;
    }

    this->gen_01_grainPos_set(v);
}

void param_06_value_set(number v) {
    v = this->param_06_value_constrain(v);
    this->param_06_value = v;
    this->sendParameter(5, false);

    if (this->param_06_value != this->param_06_lastValue) {
        {
            this->getEngine()->presetTouched();
        }

        this->param_06_lastValue = this->param_06_value;
    }

    this->gen_01_grainSize_set(v);
}

void param_07_value_set(number v) {
    v = this->param_07_value_constrain(v);
    this->param_07_value = v;
    this->sendParameter(6, false);

    if (this->param_07_value != this->param_07_lastValue) {
        {
            this->getEngine()->presetTouched();
        }

        this->param_07_lastValue = this->param_07_value;
    }

    this->gen_01_interval_set(v);
}

MillisecondTime getPatcherTime() const {
    return this->_currentTime;
}

void deallocateSignals() {
    Index i;
    this->globaltransport_tempo = freeSignal(this->globaltransport_tempo);
    this->globaltransport_state = freeSignal(this->globaltransport_state);
    this->zeroBuffer = freeSignal(this->zeroBuffer);
    this->dummyBuffer = freeSignal(this->dummyBuffer);
}

Index getMaxBlockSize() const {
    return this->maxvs;
}

number getSampleRate() const {
    return this->sr;
}

bool hasFixedVectorSize() const {
    return false;
}

void setProbingTarget(MessageTag ) {}

void fillDataRef(DataRefIndex , DataRef& ) {}

void zeroDataRef(DataRef& ref) {
    ref->setZero();
}

void allocateDataRefs() {
    this->gen_01_data_param->requestSize(6, 100);
    this->gen_01_data_param->setSampleRate(this->sr);
    this->gen_01_data_param = this->gen_01_data_param->allocateIfNeeded();

    if (this->data_param->hasRequestedSize()) {
        if (this->data_param->wantsFill())
            this->zeroDataRef(this->data_param);

        this->getEngine()->sendDataRefUpdated(0);
    }

    this->gen_01_del_inL_buffer = this->gen_01_del_inL_buffer->allocateIfNeeded();

    if (this->gen_01_del_inL_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_inL_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_inL_bufferobj);

        this->getEngine()->sendDataRefUpdated(1);
    }

    this->gen_01_del_inR_buffer = this->gen_01_del_inR_buffer->allocateIfNeeded();

    if (this->gen_01_del_inR_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_inR_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_inR_bufferobj);

        this->getEngine()->sendDataRefUpdated(2);
    }
}

void initializeObjects() {
    this->gen_01_del_inL_init();
    this->gen_01_del_inR_init();
    this->gen_01_his_index_init();
    this->gen_01_his_mix_init();
    this->gen_01_his_gain_init();
    this->gen_01_counter_0_init();
    this->gen_01_noise_4_init();
    this->gen_01_noise_7_init();
}

Index getIsMuted()  {
    return this->isMuted;
}

void setIsMuted(Index v)  {
    this->isMuted = v;
}

void onSampleRateChanged(double ) {}

void extractState(PatcherStateInterface& ) {}

void applyState() {}

void processClockEvent(MillisecondTime , ClockId , bool , ParameterValue ) {}

void processOutletAtCurrentTime(EngineLink* , OutletIndex , ParameterValue ) {}

void processOutletEvent(
    EngineLink* sender,
    OutletIndex index,
    ParameterValue value,
    MillisecondTime time
) {
    this->updateTime(time, (ENGINE*)nullptr);
    this->processOutletAtCurrentTime(sender, index, value);
}

void sendOutlet(OutletIndex index, ParameterValue value) {
    this->getEngine()->sendOutlet(this, index, value);
}

void startup() {
    this->updateTime(this->getEngine()->getCurrentTime(), (ENGINE*)nullptr);

    {
        this->scheduleParamInit(0, 0);
    }

    {
        this->scheduleParamInit(1, 0);
    }

    {
        this->scheduleParamInit(2, 0);
    }

    {
        this->scheduleParamInit(3, 0);
    }

    {
        this->scheduleParamInit(4, 0);
    }

    {
        this->scheduleParamInit(5, 0);
    }

    {
        this->scheduleParamInit(6, 0);
    }

    this->processParamInitEvents();
}

number param_01_value_constrain(number v) const {
    v = (v > 100 ? 100 : (v < 0 ? 0 : v));
    return v;
}

void gen_01_width_set(number v) {
    this->gen_01_width = v;
}

void expr_02_out1_set(number v) {
    this->expr_02_out1 = v;
    this->gen_01_width_set(this->expr_02_out1);
}

void expr_02_in1_set(number in1) {
    this->expr_02_in1 = in1;
    this->expr_02_out1_set(this->expr_02_in1 * this->expr_02_in2);//#map:*_obj-21:1
}

number param_02_value_constrain(number v) const {
    v = (v > 12 ? 12 : (v < -36 ? -36 : v));
    return v;
}

void gen_01_gain_set(number v) {
    this->gen_01_gain = v;
}

void expr_04_out1_set(number v) {
    this->expr_04_out1 = v;
    this->gen_01_gain_set(this->expr_04_out1);
}

void expr_04_in1_set(number in1) {
    this->expr_04_in1 = in1;
    this->expr_04_out1_set(rnbo_pow(10, this->expr_04_in1 * 0.05));//#map:dbtoa_obj-15:1
}

number param_03_value_constrain(number v) const {
    v = (v > 100 ? 100 : (v < 0 ? 0 : v));
    return v;
}

void gen_01_mix_set(number v) {
    this->gen_01_mix = v;
}

void expr_01_out1_set(number v) {
    this->expr_01_out1 = v;
    this->gen_01_mix_set(this->expr_01_out1);
}

void expr_01_in1_set(number in1) {
    this->expr_01_in1 = in1;
    this->expr_01_out1_set(this->expr_01_in1 * this->expr_01_in2);//#map:*_obj-22:1
}

number param_04_value_constrain(number v) const {
    v = (v > 12 ? 12 : (v < -12 ? -12 : v));
    return v;
}

void gen_01_pitch_set(number v) {
    this->gen_01_pitch = v;
}

number param_05_value_constrain(number v) const {
    v = (v > 500 ? 500 : (v < 10 ? 10 : v));
    return v;
}

void gen_01_grainPos_set(number v) {
    this->gen_01_grainPos = v;
}

number param_06_value_constrain(number v) const {
    v = (v > 500 ? 500 : (v < 10 ? 10 : v));
    return v;
}

void gen_01_grainSize_set(number v) {
    this->gen_01_grainSize = v;
}

number param_07_value_constrain(number v) const {
    v = (v > 500 ? 500 : (v < 10 ? 10 : v));
    return v;
}

void gen_01_interval_set(number v) {
    this->gen_01_interval = v;
}

void ctlin_01_outchannel_set(number ) {}

void ctlin_01_outcontroller_set(number ) {}

void fromnormalized_01_output_set(number v) {
    this->param_01_value_set(v);
}

void fromnormalized_01_input_set(number v) {
    this->fromnormalized_01_output_set(this->fromnormalized(0, v));
}

void expr_03_out1_set(number v) {
    this->expr_03_out1 = v;
    this->fromnormalized_01_input_set(this->expr_03_out1);
}

void expr_03_in1_set(number in1) {
    this->expr_03_in1 = in1;
    this->expr_03_out1_set(this->expr_03_in1 * this->expr_03_in2);//#map:expr_03:1
}

void ctlin_01_value_set(number v) {
    this->expr_03_in1_set(v);
}

void ctlin_01_midihandler(int status, int channel, int port, ConstByteArray data, Index length) {
    RNBO_UNUSED(length);
    RNBO_UNUSED(port);

    if (status == 0xB0 && (channel == this->ctlin_01_channel || this->ctlin_01_channel == -1) && (data[1] == this->ctlin_01_controller || this->ctlin_01_controller == -1)) {
        this->ctlin_01_outchannel_set(channel);
        this->ctlin_01_outcontroller_set(data[1]);
        this->ctlin_01_value_set(data[2]);
        this->ctlin_01_status = 0;
    }
}

void ctlin_02_outchannel_set(number ) {}

void ctlin_02_outcontroller_set(number ) {}

void fromnormalized_02_output_set(number v) {
    this->param_02_value_set(v);
}

void fromnormalized_02_input_set(number v) {
    this->fromnormalized_02_output_set(this->fromnormalized(1, v));
}

void expr_05_out1_set(number v) {
    this->expr_05_out1 = v;
    this->fromnormalized_02_input_set(this->expr_05_out1);
}

void expr_05_in1_set(number in1) {
    this->expr_05_in1 = in1;
    this->expr_05_out1_set(this->expr_05_in1 * this->expr_05_in2);//#map:expr_05:1
}

void ctlin_02_value_set(number v) {
    this->expr_05_in1_set(v);
}

void ctlin_02_midihandler(int status, int channel, int port, ConstByteArray data, Index length) {
    RNBO_UNUSED(length);
    RNBO_UNUSED(port);

    if (status == 0xB0 && (channel == this->ctlin_02_channel || this->ctlin_02_channel == -1) && (data[1] == this->ctlin_02_controller || this->ctlin_02_controller == -1)) {
        this->ctlin_02_outchannel_set(channel);
        this->ctlin_02_outcontroller_set(data[1]);
        this->ctlin_02_value_set(data[2]);
        this->ctlin_02_status = 0;
    }
}

void ctlin_03_outchannel_set(number ) {}

void ctlin_03_outcontroller_set(number ) {}

void fromnormalized_03_output_set(number v) {
    this->param_03_value_set(v);
}

void fromnormalized_03_input_set(number v) {
    this->fromnormalized_03_output_set(this->fromnormalized(2, v));
}

void expr_06_out1_set(number v) {
    this->expr_06_out1 = v;
    this->fromnormalized_03_input_set(this->expr_06_out1);
}

void expr_06_in1_set(number in1) {
    this->expr_06_in1 = in1;
    this->expr_06_out1_set(this->expr_06_in1 * this->expr_06_in2);//#map:expr_06:1
}

void ctlin_03_value_set(number v) {
    this->expr_06_in1_set(v);
}

void ctlin_03_midihandler(int status, int channel, int port, ConstByteArray data, Index length) {
    RNBO_UNUSED(length);
    RNBO_UNUSED(port);

    if (status == 0xB0 && (channel == this->ctlin_03_channel || this->ctlin_03_channel == -1) && (data[1] == this->ctlin_03_controller || this->ctlin_03_controller == -1)) {
        this->ctlin_03_outchannel_set(channel);
        this->ctlin_03_outcontroller_set(data[1]);
        this->ctlin_03_value_set(data[2]);
        this->ctlin_03_status = 0;
    }
}

void ctlin_04_outchannel_set(number ) {}

void ctlin_04_outcontroller_set(number ) {}

void fromnormalized_04_output_set(number v) {
    this->param_04_value_set(v);
}

void fromnormalized_04_input_set(number v) {
    this->fromnormalized_04_output_set(this->fromnormalized(3, v));
}

void expr_07_out1_set(number v) {
    this->expr_07_out1 = v;
    this->fromnormalized_04_input_set(this->expr_07_out1);
}

void expr_07_in1_set(number in1) {
    this->expr_07_in1 = in1;
    this->expr_07_out1_set(this->expr_07_in1 * this->expr_07_in2);//#map:expr_07:1
}

void ctlin_04_value_set(number v) {
    this->expr_07_in1_set(v);
}

void ctlin_04_midihandler(int status, int channel, int port, ConstByteArray data, Index length) {
    RNBO_UNUSED(length);
    RNBO_UNUSED(port);

    if (status == 0xB0 && (channel == this->ctlin_04_channel || this->ctlin_04_channel == -1) && (data[1] == this->ctlin_04_controller || this->ctlin_04_controller == -1)) {
        this->ctlin_04_outchannel_set(channel);
        this->ctlin_04_outcontroller_set(data[1]);
        this->ctlin_04_value_set(data[2]);
        this->ctlin_04_status = 0;
    }
}

void ctlin_05_outchannel_set(number ) {}

void ctlin_05_outcontroller_set(number ) {}

void fromnormalized_05_output_set(number v) {
    this->param_05_value_set(v);
}

void fromnormalized_05_input_set(number v) {
    this->fromnormalized_05_output_set(this->fromnormalized(4, v));
}

void expr_08_out1_set(number v) {
    this->expr_08_out1 = v;
    this->fromnormalized_05_input_set(this->expr_08_out1);
}

void expr_08_in1_set(number in1) {
    this->expr_08_in1 = in1;
    this->expr_08_out1_set(this->expr_08_in1 * this->expr_08_in2);//#map:expr_08:1
}

void ctlin_05_value_set(number v) {
    this->expr_08_in1_set(v);
}

void ctlin_05_midihandler(int status, int channel, int port, ConstByteArray data, Index length) {
    RNBO_UNUSED(length);
    RNBO_UNUSED(port);

    if (status == 0xB0 && (channel == this->ctlin_05_channel || this->ctlin_05_channel == -1) && (data[1] == this->ctlin_05_controller || this->ctlin_05_controller == -1)) {
        this->ctlin_05_outchannel_set(channel);
        this->ctlin_05_outcontroller_set(data[1]);
        this->ctlin_05_value_set(data[2]);
        this->ctlin_05_status = 0;
    }
}

void ctlin_06_outchannel_set(number ) {}

void ctlin_06_outcontroller_set(number ) {}

void fromnormalized_06_output_set(number v) {
    this->param_06_value_set(v);
}

void fromnormalized_06_input_set(number v) {
    this->fromnormalized_06_output_set(this->fromnormalized(5, v));
}

void expr_09_out1_set(number v) {
    this->expr_09_out1 = v;
    this->fromnormalized_06_input_set(this->expr_09_out1);
}

void expr_09_in1_set(number in1) {
    this->expr_09_in1 = in1;
    this->expr_09_out1_set(this->expr_09_in1 * this->expr_09_in2);//#map:expr_09:1
}

void ctlin_06_value_set(number v) {
    this->expr_09_in1_set(v);
}

void ctlin_06_midihandler(int status, int channel, int port, ConstByteArray data, Index length) {
    RNBO_UNUSED(length);
    RNBO_UNUSED(port);

    if (status == 0xB0 && (channel == this->ctlin_06_channel || this->ctlin_06_channel == -1) && (data[1] == this->ctlin_06_controller || this->ctlin_06_controller == -1)) {
        this->ctlin_06_outchannel_set(channel);
        this->ctlin_06_outcontroller_set(data[1]);
        this->ctlin_06_value_set(data[2]);
        this->ctlin_06_status = 0;
    }
}

void ctlin_07_outchannel_set(number ) {}

void ctlin_07_outcontroller_set(number ) {}

void fromnormalized_07_output_set(number v) {
    this->param_07_value_set(v);
}

void fromnormalized_07_input_set(number v) {
    this->fromnormalized_07_output_set(this->fromnormalized(6, v));
}

void expr_10_out1_set(number v) {
    this->expr_10_out1 = v;
    this->fromnormalized_07_input_set(this->expr_10_out1);
}

void expr_10_in1_set(number in1) {
    this->expr_10_in1 = in1;
    this->expr_10_out1_set(this->expr_10_in1 * this->expr_10_in2);//#map:expr_10:1
}

void ctlin_07_value_set(number v) {
    this->expr_10_in1_set(v);
}

void ctlin_07_midihandler(int status, int channel, int port, ConstByteArray data, Index length) {
    RNBO_UNUSED(length);
    RNBO_UNUSED(port);

    if (status == 0xB0 && (channel == this->ctlin_07_channel || this->ctlin_07_channel == -1) && (data[1] == this->ctlin_07_controller || this->ctlin_07_controller == -1)) {
        this->ctlin_07_outchannel_set(channel);
        this->ctlin_07_outcontroller_set(data[1]);
        this->ctlin_07_value_set(data[2]);
        this->ctlin_07_status = 0;
    }
}

void gen_01_perform(
    const Sample * in1,
    const Sample * in2,
    number interval,
    number grainSize,
    number grainPos,
    number width,
    number pitch,
    number mix,
    number gain,
    SampleValue * out1,
    SampleValue * out2,
    Index n
) {
    auto __gen_01_his_index_value = this->gen_01_his_index_value;
    auto __gen_01_his_gain_value = this->gen_01_his_gain_value;
    auto __gen_01_his_mix_value = this->gen_01_his_mix_value;
    Index i0;

    for (i0 = 0; i0 < (Index)n; i0++) {
        __gen_01_his_mix_value = (mix - __gen_01_his_mix_value) * 0.001 + __gen_01_his_mix_value;
        __gen_01_his_gain_value = (gain - __gen_01_his_gain_value) * 0.001 + __gen_01_his_gain_value;
        this->gen_01_del_inL_write(in1[(Index)i0]);
        this->gen_01_del_inR_write(in2[(Index)i0]);
        number mCout = 0;
        array<number, 3> result_1 = this->gen_01_counter_0_next(1, 0, this->mstosamps(interval));
        mCout = result_1[0];

        if (mCout == 1) {
            number peek_1 = 0;
            number peek_2 = 0;
            auto result_2 = this->peek_default(this->gen_01_data_param, 0, __gen_01_his_index_value);
            peek_2 = result_2[1];
            peek_1 = result_2[0];

            if (peek_1 == 0) {
                this->poke_default(this->gen_01_data_param, 1, 0, __gen_01_his_index_value, 0);

                this->poke_default(
                    this->gen_01_data_param,
                    this->mstosamps(grainSize),
                    1,
                    __gen_01_his_index_value,
                    0
                );

                auto sizeScaled_3 = this->mstosamps(grainSize);
                number widthRand_5 = this->gen_01_noise_4_next() * 0.5 * width;
                this->poke_default(this->gen_01_data_param, 0.5 + widthRand_5, 3, __gen_01_his_index_value, 0);
                this->poke_default(this->gen_01_data_param, 0.5 - widthRand_5, 4, __gen_01_his_index_value, 0);
                number pitchScaled_6 = fixnan(rnbo_pow(2, pitch / (number)12)) - 1;
                this->poke_default(this->gen_01_data_param, pitchScaled_6, 5, __gen_01_his_index_value, 0);
                __gen_01_his_index_value = this->wrap(__gen_01_his_index_value + 1, 0, 100);
                number posScaled_8 = this->mstosamps(grainPos) * rnbo_abs(this->gen_01_noise_7_next());

                this->poke_default(
                    this->gen_01_data_param,
                    posScaled_8 + this->maximum(sizeScaled_3 * pitchScaled_6, 0),
                    2,
                    __gen_01_his_index_value,
                    0
                );
            }
        }

        number outL_9 = 0;
        number outR_10 = 0;
        number totalWin_11 = 0;

        for (number i = 0; i < 100; i = i + 1) {
            number pCout = 0;
            auto result_12 = this->peek_default(this->gen_01_data_param, 0, i);
            pCout = result_12[0];

            if (pCout != 0) {
                number s = 0;
                auto result_13 = this->peek_default(this->gen_01_data_param, 1, i);
                s = result_13[0];

                if (pCout < s) {
                    number pCoutScaled_14 = (s == 0. ? 0. : pCout / s);
                    number win_15 = 0.5 * (rnbo_sin(4.7123889803847 + pCoutScaled_14 * 6.28318530717958647692) + 1);
                    number peek_3 = 0;
                    number peek_4 = 0;
                    auto result_16 = this->peek_default(this->gen_01_data_param, 2, i);
                    peek_4 = result_16[1];
                    peek_3 = result_16[0];
                    number peek_5 = 0;
                    number peek_6 = 0;
                    auto result_17 = this->peek_default(this->gen_01_data_param, 5, i);
                    peek_6 = result_17[1];
                    peek_5 = result_17[0];
                    number outputL_18 = this->gen_01_del_inL_read(peek_3 - pCout * peek_5, 0) * win_15;
                    number peek_7 = 0;
                    number peek_8 = 0;
                    auto result_19 = this->peek_default(this->gen_01_data_param, 2, i);
                    peek_8 = result_19[1];
                    peek_7 = result_19[0];
                    number peek_9 = 0;
                    number peek_10 = 0;
                    auto result_20 = this->peek_default(this->gen_01_data_param, 5, i);
                    peek_10 = result_20[1];
                    peek_9 = result_20[0];
                    number outputR_21 = this->gen_01_del_inR_read(peek_7 - pCout * peek_9, 0) * win_15;
                    number peek_11 = 0;
                    number peek_12 = 0;
                    auto result_22 = this->peek_default(this->gen_01_data_param, 3, i);
                    peek_12 = result_22[1];
                    peek_11 = result_22[0];
                    outL_9 = outL_9 + outputL_18 * peek_11;
                    number peek_13 = 0;
                    number peek_14 = 0;
                    auto result_23 = this->peek_default(this->gen_01_data_param, 4, i);
                    peek_14 = result_23[1];
                    peek_13 = result_23[0];
                    outR_10 = outR_10 + outputR_21 * peek_13;
                    this->poke_default(this->gen_01_data_param, pCout + 1, 0, i, 0);
                    totalWin_11 = totalWin_11 + win_15;
                } else {
                    this->poke_default(this->gen_01_data_param, 0, 0, i, 0);
                }
            }
        }

        number normalise_24 = fixnan(rnbo_pow(
            (this->maximum(totalWin_11, 1) == 0. ? 0. : (number)1 / this->maximum(totalWin_11, 1)),
            0.3
        )) * __gen_01_his_gain_value;

        number expr_15_25 = in1[(Index)i0] + __gen_01_his_mix_value * (outL_9 * normalise_24 - in1[(Index)i0]);
        number expr_16_26 = in2[(Index)i0] + __gen_01_his_mix_value * (outR_10 * normalise_24 - in2[(Index)i0]);
        out1[(Index)i0] = expr_15_25;
        out2[(Index)i0] = expr_16_26;
        this->gen_01_del_inL_step();
        this->gen_01_del_inR_step();
    }

    this->gen_01_his_mix_value = __gen_01_his_mix_value;
    this->gen_01_his_gain_value = __gen_01_his_gain_value;
    this->gen_01_his_index_value = __gen_01_his_index_value;
}

void stackprotect_perform(Index n) {
    RNBO_UNUSED(n);
    auto __stackprotect_count = this->stackprotect_count;
    __stackprotect_count = 0;
    this->stackprotect_count = __stackprotect_count;
}

void gen_01_del_inL_step() {
    this->gen_01_del_inL_reader++;

    if (this->gen_01_del_inL_reader >= (Int)(this->gen_01_del_inL_buffer->getSize()))
        this->gen_01_del_inL_reader = 0;
}

number gen_01_del_inL_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (Int)(this->gen_01_del_inL_buffer->getSize()) + this->gen_01_del_inL_reader - ((size > this->gen_01_del_inL__maxdelay ? this->gen_01_del_inL__maxdelay : (size < (this->gen_01_del_inL_reader != this->gen_01_del_inL_writer) ? this->gen_01_del_inL_reader != this->gen_01_del_inL_writer : size)));
        Int index1 = (Int)(rnbo_floor(r));
        number frac = r - index1;
        Int index2 = (Int)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_inL_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_inL_wrap))
        ), this->gen_01_del_inL_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_inL_wrap))
        ));
    }

    number r = (Int)(this->gen_01_del_inL_buffer->getSize()) + this->gen_01_del_inL_reader - ((size > this->gen_01_del_inL__maxdelay ? this->gen_01_del_inL__maxdelay : (size < (this->gen_01_del_inL_reader != this->gen_01_del_inL_writer) ? this->gen_01_del_inL_reader != this->gen_01_del_inL_writer : size)));
    Int index1 = (Int)(rnbo_floor(r));

    return this->gen_01_del_inL_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_inL_wrap))
    );
}

void gen_01_del_inL_write(number v) {
    this->gen_01_del_inL_writer = this->gen_01_del_inL_reader;
    this->gen_01_del_inL_buffer[(Index)this->gen_01_del_inL_writer] = v;
}

number gen_01_del_inL_next(number v, Int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_inL__maxdelay : size);
    number val = this->gen_01_del_inL_read(effectiveSize, 0);
    this->gen_01_del_inL_write(v);
    this->gen_01_del_inL_step();
    return val;
}

array<Index, 2> gen_01_del_inL_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_inL_evaluateSizeExpr(this->sr, this->vs);
        this->gen_01_del_inL_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_inL_init() {
    auto result = this->gen_01_del_inL_calcSizeInSamples();
    this->gen_01_del_inL__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_inL_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_inL_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_inL_clear() {
    this->gen_01_del_inL_buffer->setZero();
}

void gen_01_del_inL_reset() {
    auto result = this->gen_01_del_inL_calcSizeInSamples();
    this->gen_01_del_inL__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_inL_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_inL_buffer);
    this->gen_01_del_inL_wrap = this->gen_01_del_inL_buffer->getSize() - 1;
    this->gen_01_del_inL_clear();

    if (this->gen_01_del_inL_reader >= this->gen_01_del_inL__maxdelay || this->gen_01_del_inL_writer >= this->gen_01_del_inL__maxdelay) {
        this->gen_01_del_inL_reader = 0;
        this->gen_01_del_inL_writer = 0;
    }
}

void gen_01_del_inL_dspsetup() {
    this->gen_01_del_inL_reset();
}

number gen_01_del_inL_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->sr * 2;
}

number gen_01_del_inL_size() {
    return this->gen_01_del_inL__maxdelay;
}

void gen_01_del_inR_step() {
    this->gen_01_del_inR_reader++;

    if (this->gen_01_del_inR_reader >= (Int)(this->gen_01_del_inR_buffer->getSize()))
        this->gen_01_del_inR_reader = 0;
}

number gen_01_del_inR_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (Int)(this->gen_01_del_inR_buffer->getSize()) + this->gen_01_del_inR_reader - ((size > this->gen_01_del_inR__maxdelay ? this->gen_01_del_inR__maxdelay : (size < (this->gen_01_del_inR_reader != this->gen_01_del_inR_writer) ? this->gen_01_del_inR_reader != this->gen_01_del_inR_writer : size)));
        Int index1 = (Int)(rnbo_floor(r));
        number frac = r - index1;
        Int index2 = (Int)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_inR_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_inR_wrap))
        ), this->gen_01_del_inR_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_inR_wrap))
        ));
    }

    number r = (Int)(this->gen_01_del_inR_buffer->getSize()) + this->gen_01_del_inR_reader - ((size > this->gen_01_del_inR__maxdelay ? this->gen_01_del_inR__maxdelay : (size < (this->gen_01_del_inR_reader != this->gen_01_del_inR_writer) ? this->gen_01_del_inR_reader != this->gen_01_del_inR_writer : size)));
    Int index1 = (Int)(rnbo_floor(r));

    return this->gen_01_del_inR_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_inR_wrap))
    );
}

void gen_01_del_inR_write(number v) {
    this->gen_01_del_inR_writer = this->gen_01_del_inR_reader;
    this->gen_01_del_inR_buffer[(Index)this->gen_01_del_inR_writer] = v;
}

number gen_01_del_inR_next(number v, Int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_inR__maxdelay : size);
    number val = this->gen_01_del_inR_read(effectiveSize, 0);
    this->gen_01_del_inR_write(v);
    this->gen_01_del_inR_step();
    return val;
}

array<Index, 2> gen_01_del_inR_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_inR_evaluateSizeExpr(this->sr, this->vs);
        this->gen_01_del_inR_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_inR_init() {
    auto result = this->gen_01_del_inR_calcSizeInSamples();
    this->gen_01_del_inR__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_inR_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_inR_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_inR_clear() {
    this->gen_01_del_inR_buffer->setZero();
}

void gen_01_del_inR_reset() {
    auto result = this->gen_01_del_inR_calcSizeInSamples();
    this->gen_01_del_inR__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_inR_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_inR_buffer);
    this->gen_01_del_inR_wrap = this->gen_01_del_inR_buffer->getSize() - 1;
    this->gen_01_del_inR_clear();

    if (this->gen_01_del_inR_reader >= this->gen_01_del_inR__maxdelay || this->gen_01_del_inR_writer >= this->gen_01_del_inR__maxdelay) {
        this->gen_01_del_inR_reader = 0;
        this->gen_01_del_inR_writer = 0;
    }
}

void gen_01_del_inR_dspsetup() {
    this->gen_01_del_inR_reset();
}

number gen_01_del_inR_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->sr * 2;
}

number gen_01_del_inR_size() {
    return this->gen_01_del_inR__maxdelay;
}

number gen_01_his_index_getvalue() {
    return this->gen_01_his_index_value;
}

void gen_01_his_index_setvalue(number val) {
    this->gen_01_his_index_value = val;
}

void gen_01_his_index_reset() {
    this->gen_01_his_index_value = 0;
}

void gen_01_his_index_init() {
    this->gen_01_his_index_value = 0;
}

number gen_01_his_mix_getvalue() {
    return this->gen_01_his_mix_value;
}

void gen_01_his_mix_setvalue(number val) {
    this->gen_01_his_mix_value = val;
}

void gen_01_his_mix_reset() {
    this->gen_01_his_mix_value = 0;
}

void gen_01_his_mix_init() {
    this->gen_01_his_mix_value = 0;
}

number gen_01_his_gain_getvalue() {
    return this->gen_01_his_gain_value;
}

void gen_01_his_gain_setvalue(number val) {
    this->gen_01_his_gain_value = val;
}

void gen_01_his_gain_reset() {
    this->gen_01_his_gain_value = 0;
}

void gen_01_his_gain_init() {
    this->gen_01_his_gain_value = 0;
}

array<number, 3> gen_01_counter_0_next(number a, number reset, number limit) {
    RNBO_UNUSED(reset);
    RNBO_UNUSED(a);
    number carry_flag = 0;

    {
        this->gen_01_counter_0_count += 1;

        if (limit != 0) {
            if ((1 > 0 && this->gen_01_counter_0_count >= limit) || (1 < 0 && this->gen_01_counter_0_count <= limit)) {
                this->gen_01_counter_0_count = 0;
                this->gen_01_counter_0_carry += 1;
                carry_flag = 1;
            }
        }
    }

    return {this->gen_01_counter_0_count, carry_flag, this->gen_01_counter_0_carry};
}

void gen_01_counter_0_init() {
    this->gen_01_counter_0_count = 0;
}

void gen_01_counter_0_reset() {
    this->gen_01_counter_0_carry = 0;
    this->gen_01_counter_0_count = 0;
}

void gen_01_noise_4_reset() {
    xoshiro_reset(
        systemticks() + this->voice() + this->random(0, 10000),
        this->gen_01_noise_4_state
    );
}

void gen_01_noise_4_init() {
    this->gen_01_noise_4_reset();
}

void gen_01_noise_4_seed(number v) {
    xoshiro_reset(v, this->gen_01_noise_4_state);
}

number gen_01_noise_4_next() {
    return xoshiro_next(this->gen_01_noise_4_state);
}

void gen_01_noise_7_reset() {
    xoshiro_reset(
        systemticks() + this->voice() + this->random(0, 10000),
        this->gen_01_noise_7_state
    );
}

void gen_01_noise_7_init() {
    this->gen_01_noise_7_reset();
}

void gen_01_noise_7_seed(number v) {
    xoshiro_reset(v, this->gen_01_noise_7_state);
}

number gen_01_noise_7_next() {
    return xoshiro_next(this->gen_01_noise_7_state);
}

void gen_01_dspsetup(bool force) {
    if ((bool)(this->gen_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->gen_01_setupDone = true;
    this->gen_01_del_inL_dspsetup();
    this->gen_01_del_inR_dspsetup();
}

void param_01_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_01_value;
}

void param_01_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_01_value_set(preset["value"]);
}

void param_02_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_02_value;
}

void param_02_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_02_value_set(preset["value"]);
}

void param_03_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_03_value;
}

void param_03_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_03_value_set(preset["value"]);
}

void param_04_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_04_value;
}

void param_04_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_04_value_set(preset["value"]);
}

void param_05_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_05_value;
}

void param_05_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_05_value_set(preset["value"]);
}

void param_06_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_06_value;
}

void param_06_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_06_value_set(preset["value"]);
}

void param_07_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_07_value;
}

void param_07_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_07_value_set(preset["value"]);
}

void globaltransport_advance() {}

void globaltransport_dspsetup(bool ) {}

bool stackprotect_check() {
    this->stackprotect_count++;

    if (this->stackprotect_count > 128) {
        console->log("STACK OVERFLOW DETECTED - stopped processing branch !");
        return true;
    }

    return false;
}

Index getPatcherSerial() const {
    return 0;
}

void sendParameter(ParameterIndex index, bool ignoreValue) {
    this->getEngine()->notifyParameterValueChanged(index, (ignoreValue ? 0 : this->getParameterValue(index)), ignoreValue);
}

void scheduleParamInit(ParameterIndex index, Index order) {
    this->paramInitIndices->push(index);
    this->paramInitOrder->push(order);
}

void processParamInitEvents() {
    this->listquicksort(
        this->paramInitOrder,
        this->paramInitIndices,
        0,
        (int)(this->paramInitOrder->length - 1),
        true
    );

    for (Index i = 0; i < this->paramInitOrder->length; i++) {
        this->getEngine()->scheduleParameterBang(this->paramInitIndices[i], 0);
    }
}

void updateTime(MillisecondTime time, EXTERNALENGINE* engine, bool inProcess = false) {
    RNBO_UNUSED(inProcess);
    RNBO_UNUSED(engine);
    this->_currentTime = time;
    auto offset = rnbo_fround(this->msToSamps(time - this->getEngine()->getCurrentTime(), this->sr));

    if (offset >= (SampleIndex)(this->vs))
        offset = (SampleIndex)(this->vs) - 1;

    if (offset < 0)
        offset = 0;

    this->sampleOffsetIntoNextAudioBuffer = (Index)(offset);
}

void assign_defaults()
{
    gen_01_in1 = 0;
    gen_01_in2 = 0;
    gen_01_interval = 100;
    gen_01_grainSize = 100;
    gen_01_grainPos = 100;
    gen_01_width = 0.5;
    gen_01_pitch = 0;
    gen_01_mix = 0.5;
    gen_01_gain = 0;
    expr_01_in1 = 0;
    expr_01_in2 = 0.01;
    expr_01_out1 = 0;
    expr_02_in1 = 0;
    expr_02_in2 = 0.01;
    expr_02_out1 = 0;
    param_01_value = 50;
    expr_04_in1 = 0;
    expr_04_out1 = 0;
    param_02_value = 0;
    param_03_value = 50;
    param_04_value = 0;
    param_05_value = 100;
    param_06_value = 100;
    param_07_value = 100;
    ctlin_01_input = 0;
    ctlin_01_controller = 0;
    ctlin_01_channel = -1;
    expr_03_in1 = 0;
    expr_03_in2 = 0.007874015748;
    expr_03_out1 = 0;
    ctlin_02_input = 0;
    ctlin_02_controller = 0;
    ctlin_02_channel = -1;
    expr_05_in1 = 0;
    expr_05_in2 = 0.007874015748;
    expr_05_out1 = 0;
    ctlin_03_input = 0;
    ctlin_03_controller = 0;
    ctlin_03_channel = -1;
    expr_06_in1 = 0;
    expr_06_in2 = 0.007874015748;
    expr_06_out1 = 0;
    ctlin_04_input = 0;
    ctlin_04_controller = 0;
    ctlin_04_channel = -1;
    expr_07_in1 = 0;
    expr_07_in2 = 0.007874015748;
    expr_07_out1 = 0;
    ctlin_05_input = 0;
    ctlin_05_controller = 0;
    ctlin_05_channel = -1;
    expr_08_in1 = 0;
    expr_08_in2 = 0.007874015748;
    expr_08_out1 = 0;
    ctlin_06_input = 0;
    ctlin_06_controller = 0;
    ctlin_06_channel = -1;
    expr_09_in1 = 0;
    expr_09_in2 = 0.007874015748;
    expr_09_out1 = 0;
    ctlin_07_input = 0;
    ctlin_07_controller = 0;
    ctlin_07_channel = -1;
    expr_10_in1 = 0;
    expr_10_in2 = 0.007874015748;
    expr_10_out1 = 0;
    _currentTime = 0;
    audioProcessSampleCount = 0;
    sampleOffsetIntoNextAudioBuffer = 0;
    zeroBuffer = nullptr;
    dummyBuffer = nullptr;
    didAllocateSignals = 0;
    vs = 0;
    maxvs = 0;
    sr = 44100;
    invsr = 0.000022675736961451248;
    gen_01_del_inL__maxdelay = 0;
    gen_01_del_inL_sizemode = 0;
    gen_01_del_inL_wrap = 0;
    gen_01_del_inL_reader = 0;
    gen_01_del_inL_writer = 0;
    gen_01_del_inR__maxdelay = 0;
    gen_01_del_inR_sizemode = 0;
    gen_01_del_inR_wrap = 0;
    gen_01_del_inR_reader = 0;
    gen_01_del_inR_writer = 0;
    gen_01_his_index_value = 0;
    gen_01_his_mix_value = 0;
    gen_01_his_gain_value = 0;
    gen_01_counter_0_carry = 0;
    gen_01_counter_0_count = 0;
    gen_01_setupDone = false;
    param_01_lastValue = 0;
    param_02_lastValue = 0;
    param_03_lastValue = 0;
    param_04_lastValue = 0;
    param_05_lastValue = 0;
    param_06_lastValue = 0;
    param_07_lastValue = 0;
    ctlin_01_status = 0;
    ctlin_01_byte1 = -1;
    ctlin_01_inchan = 0;
    ctlin_02_status = 0;
    ctlin_02_byte1 = -1;
    ctlin_02_inchan = 0;
    ctlin_03_status = 0;
    ctlin_03_byte1 = -1;
    ctlin_03_inchan = 0;
    ctlin_04_status = 0;
    ctlin_04_byte1 = -1;
    ctlin_04_inchan = 0;
    ctlin_05_status = 0;
    ctlin_05_byte1 = -1;
    ctlin_05_inchan = 0;
    ctlin_06_status = 0;
    ctlin_06_byte1 = -1;
    ctlin_06_inchan = 0;
    ctlin_07_status = 0;
    ctlin_07_byte1 = -1;
    ctlin_07_inchan = 0;
    globaltransport_tempo = nullptr;
    globaltransport_state = nullptr;
    stackprotect_count = 0;
    _voiceIndex = 0;
    _noteNumber = 0;
    isMuted = 1;
}

    // data ref strings
    struct DataRefStrings {
    	static constexpr auto& name0 = "data_param";
    	static constexpr auto& file0 = "";
    	static constexpr auto& tag0 = "buffer~";
    	static constexpr auto& name1 = "gen_01_del_inL_bufferobj";
    	static constexpr auto& file1 = "";
    	static constexpr auto& tag1 = "buffer~";
    	static constexpr auto& name2 = "gen_01_del_inR_bufferobj";
    	static constexpr auto& file2 = "";
    	static constexpr auto& tag2 = "buffer~";
    	DataRefStrings* operator->() { return this; }
    	const DataRefStrings* operator->() const { return this; }
    };

    DataRefStrings dataRefStrings;

// member variables

    number gen_01_in1;
    number gen_01_in2;
    number gen_01_interval;
    number gen_01_grainSize;
    number gen_01_grainPos;
    number gen_01_width;
    number gen_01_pitch;
    number gen_01_mix;
    number gen_01_gain;
    number expr_01_in1;
    number expr_01_in2;
    number expr_01_out1;
    number expr_02_in1;
    number expr_02_in2;
    number expr_02_out1;
    number param_01_value;
    number expr_04_in1;
    number expr_04_out1;
    number param_02_value;
    number param_03_value;
    number param_04_value;
    number param_05_value;
    number param_06_value;
    number param_07_value;
    number ctlin_01_input;
    number ctlin_01_controller;
    number ctlin_01_channel;
    number expr_03_in1;
    number expr_03_in2;
    number expr_03_out1;
    number ctlin_02_input;
    number ctlin_02_controller;
    number ctlin_02_channel;
    number expr_05_in1;
    number expr_05_in2;
    number expr_05_out1;
    number ctlin_03_input;
    number ctlin_03_controller;
    number ctlin_03_channel;
    number expr_06_in1;
    number expr_06_in2;
    number expr_06_out1;
    number ctlin_04_input;
    number ctlin_04_controller;
    number ctlin_04_channel;
    number expr_07_in1;
    number expr_07_in2;
    number expr_07_out1;
    number ctlin_05_input;
    number ctlin_05_controller;
    number ctlin_05_channel;
    number expr_08_in1;
    number expr_08_in2;
    number expr_08_out1;
    number ctlin_06_input;
    number ctlin_06_controller;
    number ctlin_06_channel;
    number expr_09_in1;
    number expr_09_in2;
    number expr_09_out1;
    number ctlin_07_input;
    number ctlin_07_controller;
    number ctlin_07_channel;
    number expr_10_in1;
    number expr_10_in2;
    number expr_10_out1;
    MillisecondTime _currentTime;
    ENGINE _internalEngine;
    UInt64 audioProcessSampleCount;
    Index sampleOffsetIntoNextAudioBuffer;
    signal zeroBuffer;
    signal dummyBuffer;
    bool didAllocateSignals;
    Index vs;
    Index maxvs;
    number sr;
    number invsr;
    Float64BufferRef gen_01_del_inL_buffer;
    Index gen_01_del_inL__maxdelay;
    Int gen_01_del_inL_sizemode;
    Index gen_01_del_inL_wrap;
    Int gen_01_del_inL_reader;
    Int gen_01_del_inL_writer;
    Float64BufferRef gen_01_del_inR_buffer;
    Index gen_01_del_inR__maxdelay;
    Int gen_01_del_inR_sizemode;
    Index gen_01_del_inR_wrap;
    Int gen_01_del_inR_reader;
    Int gen_01_del_inR_writer;
    SampleBufferRef gen_01_data_param;
    number gen_01_his_index_value;
    number gen_01_his_mix_value;
    number gen_01_his_gain_value;
    Int gen_01_counter_0_carry;
    number gen_01_counter_0_count;
    UInt gen_01_noise_4_state[4] = { };
    UInt gen_01_noise_7_state[4] = { };
    bool gen_01_setupDone;
    number param_01_lastValue;
    number param_02_lastValue;
    number param_03_lastValue;
    number param_04_lastValue;
    number param_05_lastValue;
    number param_06_lastValue;
    number param_07_lastValue;
    Int ctlin_01_status;
    Int ctlin_01_byte1;
    Int ctlin_01_inchan;
    Int ctlin_02_status;
    Int ctlin_02_byte1;
    Int ctlin_02_inchan;
    Int ctlin_03_status;
    Int ctlin_03_byte1;
    Int ctlin_03_inchan;
    Int ctlin_04_status;
    Int ctlin_04_byte1;
    Int ctlin_04_inchan;
    Int ctlin_05_status;
    Int ctlin_05_byte1;
    Int ctlin_05_inchan;
    Int ctlin_06_status;
    Int ctlin_06_byte1;
    Int ctlin_06_inchan;
    Int ctlin_07_status;
    Int ctlin_07_byte1;
    Int ctlin_07_inchan;
    signal globaltransport_tempo;
    signal globaltransport_state;
    number stackprotect_count;
    DataRef data_param;
    DataRef gen_01_del_inL_bufferobj;
    DataRef gen_01_del_inR_bufferobj;
    Index _voiceIndex;
    Int _noteNumber;
    Index isMuted;
    indexlist paramInitIndices;
    indexlist paramInitOrder;
    bool _isInitialized = false;
};

static PatcherInterface* createRnboGranular()
{
    return new RnboGranular<EXTERNALENGINE>();
}

#ifndef RNBO_NO_PATCHERFACTORY
extern "C" PatcherFactoryFunctionPtr GetPatcherFactoryFunction()
#else
extern "C" PatcherFactoryFunctionPtr RnboGranularFactoryFunction()
#endif
{
    return createRnboGranular;
}

#ifndef RNBO_NO_PATCHERFACTORY
extern "C" void SetLogger(Logger* logger)
#else
void RnboGranularSetLogger(Logger* logger)
#endif
{
    console = logger;
}

} // end RNBO namespace


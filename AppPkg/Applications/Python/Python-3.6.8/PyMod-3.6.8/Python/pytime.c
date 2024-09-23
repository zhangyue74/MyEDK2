/** @file
  Time related functions

  Copyright (c) 2010 - 2021, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials are licensed and made available under
  the terms and conditions of the BSD License that accompanies this distribution.
  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#include "Python.h"
#ifdef MS_WINDOWS
#include <windows.h>
#endif

#if defined(__APPLE__)
#include <mach/mach_time.h>   /* mach_absolute_time(), mach_timebase_info() */
#endif

#define _PyTime_check_mul_overflow(a, b) \
    (assert(b > 0), \
     (_PyTime_t)(a) < _PyTime_MIN / (_PyTime_t)(b) \
     || _PyTime_MAX / (_PyTime_t)(b) < (_PyTime_t)(a))

/* To millisecond (10^-3) */
#define SEC_TO_MS 1000

/* To microseconds (10^-6) */
#define MS_TO_US 1000
#define SEC_TO_US (SEC_TO_MS * MS_TO_US)

/* To nanoseconds (10^-9) */
#define US_TO_NS 1000
#define MS_TO_NS (MS_TO_US * US_TO_NS)
#define SEC_TO_NS (SEC_TO_MS * MS_TO_NS)

/* Conversion from nanoseconds */
#define NS_TO_MS (1000 * 1000)
#define NS_TO_US (1000)

static void
error_time_t_overflow(void)
{
    PyErr_SetString(PyExc_OverflowError,
                    "timestamp out of range for platform time_t");
}

time_t
_PyLong_AsTime_t(PyObject *obj)
{
#if SIZEOF_TIME_T == SIZEOF_LONG_LONG
    long long val;
    val = PyLong_AsLongLong(obj);
#else
    long val;
    Py_BUILD_ASSERT(sizeof(time_t) <= sizeof(long));
    val = PyLong_AsLong(obj);
#endif
    if (val == -1 && PyErr_Occurred()) {
        if (PyErr_ExceptionMatches(PyExc_OverflowError))
            error_time_t_overflow();
        return -1;
    }
    return (time_t)val;
}

PyObject *
_PyLong_FromTime_t(time_t t)
{
#if SIZEOF_TIME_T == SIZEOF_LONG_LONG
    return PyLong_FromLongLong((long long)t);
#else
    Py_BUILD_ASSERT(sizeof(time_t) <= sizeof(long));
    return PyLong_FromLong((long)t);
#endif
}

/* Round to nearest with ties going to nearest even integer
   (_PyTime_ROUND_HALF_EVEN) */
static double
_PyTime_RoundHalfEven(double x)
{
    double rounded = round(x);
    if (fabs(x-rounded) == 0.5)
        /* halfway case: round to even */
        rounded = 2.0*round(x/2.0);
    return rounded;
}

static double
_PyTime_Round(double x, _PyTime_round_t round)
{
    /* volatile avoids optimization changing how numbers are rounded */
    volatile double d;

    d = x;
    if (round == _PyTime_ROUND_HALF_EVEN){
        d = _PyTime_RoundHalfEven(d);
    }
    else if (round == _PyTime_ROUND_CEILING){
        d = ceil(d);
    }
    else if (round == _PyTime_ROUND_FLOOR) {
        d = floor(d);
    }
    else {
        assert(round == _PyTime_ROUND_UP);
        d = (d >= 0.0) ? ceil(d) : floor(d);
    }
    return d;
}

static int
_PyTime_DoubleToDenominator(double d, time_t *sec, long *numerator,
                            double denominator, _PyTime_round_t round)
{
    double intpart;
    /* volatile avoids optimization changing how numbers are rounded */
    volatile double floatpart;

    floatpart = modf(d, &intpart);

    floatpart *= denominator;
    floatpart = _PyTime_Round(floatpart, round);
    if (floatpart >= denominator) {
        floatpart -= denominator;
        intpart += 1.0;
    }
    else if (floatpart < 0) {
        floatpart += denominator;
        intpart -= 1.0;
    }
    assert(0.0 <= floatpart && floatpart < denominator);

    if (!_Py_InIntegralTypeRange(time_t, intpart)) {
        error_time_t_overflow();
        return -1;
    }
    *sec = (time_t)intpart;
    *numerator = (long)floatpart;

    return 0;
}

static int
_PyTime_ObjectToDenominator(PyObject *obj, time_t *sec, long *numerator,
                            double denominator, _PyTime_round_t round)
{
    assert(denominator <= (double)LONG_MAX);

    if (PyFloat_Check(obj)) {
        double d = PyFloat_AsDouble(obj);
        if (Py_IS_NAN(d)) {
            *numerator = 0;
            PyErr_SetString(PyExc_ValueError, "Invalid value NaN (not a number)");
            return -1;
        }
        return _PyTime_DoubleToDenominator(d, sec, numerator,
                                           denominator, round);
    }
    else {
        *sec = _PyLong_AsTime_t(obj);
        *numerator = 0;
        if (*sec == (time_t)-1 && PyErr_Occurred())
            return -1;
        return 0;
    }
}

int
_PyTime_ObjectToTime_t(PyObject *obj, time_t *sec, _PyTime_round_t round)
{
    if (PyFloat_Check(obj)) {
        double intpart;
        /* volatile avoids optimization changing how numbers are rounded */
        volatile double d;

        d = PyFloat_AsDouble(obj);
        if (Py_IS_NAN(d)) {
            PyErr_SetString(PyExc_ValueError, "Invalid value NaN (not a number)");
            return -1;
        }

        d = _PyTime_Round(d, round);
        (void)modf(d, &intpart);

        if (!_Py_InIntegralTypeRange(time_t, intpart)) {
            error_time_t_overflow();
            return -1;
        }
        *sec = (time_t)intpart;
        return 0;
    }
    else {
        *sec = _PyLong_AsTime_t(obj);
        if (*sec == (time_t)-1 && PyErr_Occurred())
            return -1;
        return 0;
    }
}

int
_PyTime_ObjectToTimespec(PyObject *obj, time_t *sec, long *nsec,
                         _PyTime_round_t round)
{
    int res;
    res = _PyTime_ObjectToDenominator(obj, sec, nsec, 1e9, round);
    if (res == 0) {
        assert(0 <= *nsec && *nsec < SEC_TO_NS);
    }
    return res;
}

int
_PyTime_ObjectToTimeval(PyObject *obj, time_t *sec, long *usec,
                        _PyTime_round_t round)
{
    int res;
    res = _PyTime_ObjectToDenominator(obj, sec, usec, 1e6, round);
    if (res == 0) {
        assert(0 <= *usec && *usec < SEC_TO_US);
    }
    return res;
}

static void
_PyTime_overflow(void)
{
    PyErr_SetString(PyExc_OverflowError,
                    "timestamp too large to convert to C _PyTime_t");
}

_PyTime_t
_PyTime_FromSeconds(int seconds)
{
    _PyTime_t t;
    t = (_PyTime_t)seconds;
    /* ensure that integer overflow cannot happen, int type should have 32
       bits, whereas _PyTime_t type has at least 64 bits (SEC_TO_MS takes 30
       bits). */
    Py_BUILD_ASSERT(INT_MAX <= _PyTime_MAX / SEC_TO_NS);
    Py_BUILD_ASSERT(INT_MIN >= _PyTime_MIN / SEC_TO_NS);
    assert((t >= 0 && t <= _PyTime_MAX / SEC_TO_NS)
           || (t < 0 && t >= _PyTime_MIN / SEC_TO_NS));
    t *= SEC_TO_NS;
    return t;
}

_PyTime_t
_PyTime_FromNanoseconds(long long ns)
{
    _PyTime_t t;
    Py_BUILD_ASSERT(sizeof(long long) <= sizeof(_PyTime_t));
    t = Py_SAFE_DOWNCAST(ns, long long, _PyTime_t);
    return t;
}

static int
_PyTime_FromTimespec(_PyTime_t *tp, struct timespec *ts, int raise)
{
    _PyTime_t t;
    int res = 0;

    Py_BUILD_ASSERT(sizeof(ts->tv_sec) <= sizeof(_PyTime_t));
    t = (_PyTime_t)ts->tv_sec;

    if (_PyTime_check_mul_overflow(t, SEC_TO_NS)) {
        if (raise)
            _PyTime_overflow();
        res = -1;
    }
    t = t * SEC_TO_NS;

    t += ts->tv_nsec;

    *tp = t;
    return res;
}


#ifdef HAVE_CLOCK_GETTIME
static int
_PyTime_FromTimespec(_PyTime_t *tp, struct timespec *ts, int raise)
{
    _PyTime_t t;
    int res = 0;

    Py_BUILD_ASSERT(sizeof(ts->tv_sec) <= sizeof(_PyTime_t));
    t = (_PyTime_t)ts->tv_sec;

    if (_PyTime_check_mul_overflow(t, SEC_TO_NS)) {
        if (raise)
            _PyTime_overflow();
        res = -1;
    }
    t = t * SEC_TO_NS;

    t += ts->tv_nsec;

    *tp = t;
    return res;
}
#elif !defined(MS_WINDOWS)
static int
_PyTime_FromTimeval(_PyTime_t *tp, struct timeval *tv, int raise)
{
    _PyTime_t t;
    int res = 0;

    Py_BUILD_ASSERT(sizeof(tv->tv_sec) <= sizeof(_PyTime_t));
    t = (_PyTime_t)tv->tv_sec;

    if (_PyTime_check_mul_overflow(t, SEC_TO_NS)) {
        if (raise)
            _PyTime_overflow();
        res = -1;
    }
    t = t * SEC_TO_NS;

    t += (_PyTime_t)tv->tv_usec * US_TO_NS;

    *tp = t;
    return res;
}
#endif

static int
_PyTime_FromFloatObject(_PyTime_t *t, double value, _PyTime_round_t round,
                        long unit_to_ns)
{
    /* volatile avoids optimization changing how numbers are rounded */
    volatile double d;

    /* convert to a number of nanoseconds */
    d = value;
    d *= (double)unit_to_ns;
    d = _PyTime_Round(d, round);

    if (!_Py_InIntegralTypeRange(_PyTime_t, d)) {
        _PyTime_overflow();
        return -1;
    }
    *t = (_PyTime_t)d;
    return 0;
}

static int
_PyTime_FromObject(_PyTime_t *t, PyObject *obj, _PyTime_round_t round,
                   long unit_to_ns)
{
    if (PyFloat_Check(obj)) {
        double d;
        d = PyFloat_AsDouble(obj);
        if (Py_IS_NAN(d)) {
            PyErr_SetString(PyExc_ValueError, "Invalid value NaN (not a number)");
            return -1;
        }
        return _PyTime_FromFloatObject(t, d, round, unit_to_ns);
    }
    else {
        long long sec;
        Py_BUILD_ASSERT(sizeof(long long) <= sizeof(_PyTime_t));

        sec = PyLong_AsLongLong(obj);
        if (sec == -1 && PyErr_Occurred()) {
            if (PyErr_ExceptionMatches(PyExc_OverflowError))
                _PyTime_overflow();
            return -1;
        }

        if (_PyTime_check_mul_overflow(sec, unit_to_ns)) {
            _PyTime_overflow();
            return -1;
        }
        *t = sec * unit_to_ns;
        return 0;
    }
}

int
_PyTime_FromSecondsObject(_PyTime_t *t, PyObject *obj, _PyTime_round_t round)
{
    return _PyTime_FromObject(t, obj, round, SEC_TO_NS);
}

int
_PyTime_FromMillisecondsObject(_PyTime_t *t, PyObject *obj, _PyTime_round_t round)
{
    return _PyTime_FromObject(t, obj, round, MS_TO_NS);
}

double
_PyTime_AsSecondsDouble(_PyTime_t t)
{
    /* volatile avoids optimization changing how numbers are rounded */
    volatile double d;

    if (t % SEC_TO_NS == 0) {
        _PyTime_t secs;
        /* Divide using integers to avoid rounding issues on the integer part.
           1e-9 cannot be stored exactly in IEEE 64-bit. */
        secs = t / SEC_TO_NS;
        d = (double)secs;
    }
    else {
        d = (double)t;
        d /= 1e9;
    }
    return d;
}

PyObject *
_PyTime_AsNanosecondsObject(_PyTime_t t)
{
    Py_BUILD_ASSERT(sizeof(long long) >= sizeof(_PyTime_t));
    return PyLong_FromLongLong((long long)t);
}

static _PyTime_t
_PyTime_Divide(const _PyTime_t t, const _PyTime_t k,
               const _PyTime_round_t round)
{
    assert(k > 1);
    if (round == _PyTime_ROUND_HALF_EVEN) {
        _PyTime_t x, r, abs_r;
        x = t / k;
        r = t % k;
        abs_r = Py_ABS(r);
        if (abs_r > k / 2 || (abs_r == k / 2 && (Py_ABS(x) & 1))) {
            if (t >= 0)
                x++;
            else
                x--;
        }
        return x;
    }
    else if (round == _PyTime_ROUND_CEILING) {
        if (t >= 0){
            return (t + k - 1) / k;
        }
        else{
            return t / k;
        }
    }
    else if (round == _PyTime_ROUND_FLOOR){
        if (t >= 0) {
            return t / k;
        }
        else{
            return (t - (k - 1)) / k;
        }
    }
    else {
        assert(round == _PyTime_ROUND_UP);
        if (t >= 0) {
            return (t + k - 1) / k;
        }
        else {
            return (t - (k - 1)) / k;
        }
    }
}

_PyTime_t
_PyTime_AsMilliseconds(_PyTime_t t, _PyTime_round_t round)
{
    return _PyTime_Divide(t, NS_TO_MS, round);
}

_PyTime_t
_PyTime_AsMicroseconds(_PyTime_t t, _PyTime_round_t round)
{
    return _PyTime_Divide(t, NS_TO_US, round);
}

static int
_PyTime_AsTimeval_impl(_PyTime_t t, _PyTime_t *p_secs, int *p_us,
                       _PyTime_round_t round)
{
    _PyTime_t secs, ns;
    int usec;
    int res = 0;

    secs = t / SEC_TO_NS;
    ns = t % SEC_TO_NS;

    usec = (int)_PyTime_Divide(ns, US_TO_NS, round);
    if (usec < 0) {
        usec += SEC_TO_US;
        if (secs != _PyTime_MIN)
            secs -= 1;
        else
            res = -1;
    }
    else if (usec >= SEC_TO_US) {
        usec -= SEC_TO_US;
        if (secs != _PyTime_MAX)
            secs += 1;
        else
            res = -1;
    }
    assert(0 <= usec && usec < SEC_TO_US);

    *p_secs = secs;
    *p_us = usec;

    return res;
}

static int
_PyTime_AsTimevalStruct_impl(_PyTime_t t, struct timeval *tv,
                             _PyTime_round_t round, int raise)
{
    _PyTime_t secs, secs2;
    int us;
    int res;

    res = _PyTime_AsTimeval_impl(t, &secs, &us, round);

#ifdef MS_WINDOWS
    tv->tv_sec = (long)secs;
#else
    tv->tv_sec = secs;
#endif
    tv->tv_usec = us;

    secs2 = (_PyTime_t)tv->tv_sec;
    if (res < 0 || secs2 != secs) {
        if (raise)
            error_time_t_overflow();
        return -1;
    }
    return 0;
}

int
_PyTime_AsTimeval(_PyTime_t t, struct timeval *tv, _PyTime_round_t round)
{
    return _PyTime_AsTimevalStruct_impl(t, tv, round, 1);
}

int
_PyTime_AsTimeval_noraise(_PyTime_t t, struct timeval *tv, _PyTime_round_t round)
{
    return _PyTime_AsTimevalStruct_impl(t, tv, round, 0);
}

int
_PyTime_AsTimevalTime_t(_PyTime_t t, time_t *p_secs, int *us,
                        _PyTime_round_t round)
{
    _PyTime_t secs;
    int res;

    res = _PyTime_AsTimeval_impl(t, &secs, us, round);

    *p_secs = secs;

    if (res < 0 || (_PyTime_t)*p_secs != secs) {
        error_time_t_overflow();
        return -1;
    }
    return 0;
}


#if defined(HAVE_CLOCK_GETTIME) || defined(HAVE_KQUEUE)
int
_PyTime_AsTimespec(_PyTime_t t, struct timespec *ts)
{
    _PyTime_t secs, nsec;

    secs = t / SEC_TO_NS;
    nsec = t % SEC_TO_NS;
    if (nsec < 0) {
        nsec += SEC_TO_NS;
        secs -= 1;
    }
    ts->tv_sec = (time_t)secs;
    assert(0 <= nsec && nsec < SEC_TO_NS);
    ts->tv_nsec = nsec;

    if ((_PyTime_t)ts->tv_sec != secs) {
        error_time_t_overflow();
        return -1;
    }
    return 0;
}
#endif

static int
pygettimeofday(_PyTime_t *tp, _Py_clock_info_t *info, int raise)
{
    int err;
    struct timeval tv;

    assert(info == NULL || raise);
    err = gettimeofday(&tv, (struct timezone *)NULL);
    if (err) {
        if (raise)
            PyErr_SetFromErrno(PyExc_OSError);
        return -1;
    }
    if (_PyTime_FromTimeval(tp, &tv, raise) < 0)
        return -1;

    if (info) {
        info->implementation = "gettimeofday()";
        info->resolution = 1e-6;
        info->monotonic = 0;
        info->adjustable = 1;
    }
    return 0;
}

_PyTime_t
_PyTime_GetSystemClock(void)
{
    _PyTime_t t;
    if (pygettimeofday(&t, NULL, 0) < 0) {
        /* should not happen, _PyTime_Init() checked the clock at startup */
        assert(0);

        /* use a fixed value instead of a random value from the stack */
        t = 0;
    }
    return t;
}

int
_PyTime_GetSystemClockWithInfo(_PyTime_t *t, _Py_clock_info_t *info)
{
    return pygettimeofday(t, info, 1);
}

static int
pymonotonic(_PyTime_t *tp, _Py_clock_info_t *info, int raise)
{
    struct timespec ts;

    assert(info == NULL || raise);

    if (info) {
        info->implementation = "gettimeofday()";
        info->resolution = 1e-6;
        info->monotonic = 0;
        info->adjustable = 1;
    }

    if (_PyTime_FromTimespec(tp, &ts, raise) < 0)
        return -1;
    return 0;
}

_PyTime_t
_PyTime_GetMonotonicClock(void)
{
    _PyTime_t t;
    if (pymonotonic(&t, NULL, 0) < 0) {
        /* should not happen, _PyTime_Init() checked that monotonic clock at
           startup */
        assert(0);

        /* use a fixed value instead of a random value from the stack */
        t = 0;
    }
    return t;
}

int
_PyTime_GetMonotonicClockWithInfo(_PyTime_t *tp, _Py_clock_info_t *info)
{
    return pymonotonic(tp, info, 1);
}

int
_PyTime_Init(void)
{
    _PyTime_t t;

    /* ensure that the system clock works */
    if (_PyTime_GetSystemClockWithInfo(&t, NULL) < 0)
        return -1;

    /* ensure that the operating system provides a monotonic clock */
    if (_PyTime_GetMonotonicClockWithInfo(&t, NULL) < 0)
        return -1;

    return 0;
}

int
_PyTime_localtime(time_t t, struct tm *tm)
{
#ifdef MS_WINDOWS
    int error;

    error = localtime_s(tm, &t);
    if (error != 0) {
        errno = error;
        PyErr_SetFromErrno(PyExc_OSError);
        return -1;
    }
    return 0;
#else /* !MS_WINDOWS */
    struct tm *temp = NULL;
    if ((temp = localtime(&t)) == NULL) {
#ifdef EINVAL
        if (errno == 0)
            errno = EINVAL;
#endif
        PyErr_SetFromErrno(PyExc_OSError);
        return -1;
    }
	*tm = *temp;
    return 0;
#endif /* MS_WINDOWS */
}

int
_PyTime_gmtime(time_t t, struct tm *tm)
{
#ifdef MS_WINDOWS
    int error;

    error = gmtime_s(tm, &t);
    if (error != 0) {
        errno = error;
        PyErr_SetFromErrno(PyExc_OSError);
        return -1;
    }
    return 0;
#else /* !MS_WINDOWS */
    struct tm *temp = NULL;
    if ((temp = gmtime(&t)) == NULL) {
#ifdef EINVAL
        if (errno == 0)
            errno = EINVAL;
#endif
        PyErr_SetFromErrno(PyExc_OSError);
        return -1;
    }
	*tm = *temp;
    return 0;
#endif /* MS_WINDOWS */
}

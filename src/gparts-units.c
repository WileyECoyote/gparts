/* gEDA - GPL Electronic Design Automation
 * gparts - gEDA Parts Manager
 * Copyright (C) 2010 Edward C. Hennessy
 * Copyright (C) 2010 gEDA Contributors (see ChangeLog for details)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
 */

/*! \file gparts-units.c
 */

#include <math.h>
#include <glib.h>
#include <glib-object.h>

#include "gparts-units.h"

/*! \todo Below needs to work with all compilers.  The following will work in
 * GCC even without C99 support, but it will issue a warning.
 */
//#ifdef __STDC__VERSION__
//#if __STDC_VERSION__ >= 199901L
#define GPARTS_SYMBOL_CELCIUS "\u00B0C"
#define GPARTS_SYMBOL_MICRO   "\u00B5"
#define GPARTS_SYMBOL_OHM     "\u2126"
//#endif
//#else
//#define GPARTS_SYMBOL_MICRO "u"
//#define GPARTS_SYMBOL_OHM   ""
//#endif

typedef struct _GPartsUnitsFormat GPartsUnitsFormat;
typedef struct _GPartsUnitsMetricPrefix GPartsUnitsMetricPrefix;

struct _GPartsUnits
{
    double         value;
    GValueTransform func;
    const char     *symbol;
};

struct _GPartsUnitsFormat
{
    const char *string;
    float       value;
};

struct _GPartsUnitsMetricPrefix
{
    const char *prefix;
    const char *symbol;
    double      value;
};

/*
 * Three significant digit table.
 *
 */
static const GPartsUnitsFormat gparts_units_format[] =
{
    { "%.0f %s%s", 1000 },
    { "%.0f %s%s",  100 },
    { "%.1f %s%s",   10 },
    { "%.2f %s%s",    1 },
    { "%.0f %s%s",    0 }
};

static const GPartsUnitsMetricPrefix gparts_units_metric_prefixes[] =
{
    { "yotta",  "Y",                   1e24 },
    { "zetta",  "Z",                   1e21 },
    { "exa",    "E",                   1e18 },
    { "peta",   "P",                   1e15 },
    { "tera",   "T",                   1e12 },
    { "giga",   "G",                    1e9 },
    { "mega",   "M",                    1e6 },
    { "kilo",   "k",                    1e3 },
    { "",       "",                       1 },
    { "milli",  "m",                   1e-3 },
    { "micro",  GPARTS_SYMBOL_MICRO,   1e-6 },
    { "nano",   "n",                   1e-9 },
    { "pico",   "p",                  1e-12 },
    { "femto",  "f",                  1e-15 },
    { "atto",   "a",                  1e-18 },
    { "zepto",  "z",                  1e-21 },
    { "yocto",  "y",                  1e-24 },
    { "",       "",                       0 },
};


static const char*
gparts_units_find_format(double value);

static const GPartsUnitsMetricPrefix*
gparts_units_find_metric_prefix(double value);

static void
gparts_units_transform(const GValue *src_value, GValue *dest_value);

static void
gparts_units_transform_percent(const GValue *src_value, GValue *dest_value);

static void
gparts_units_transform_pp(const GValue *src_value, GValue *dest_value);

static void
gparts_units_transform_std(const GValue *src_value, GValue *dest_value);



GPartsUnits*
gparts_units_copy(const GPartsUnits *units)
{
    return GPARTS_UNITS(g_memdup(units, sizeof(GPartsUnits)));
}

static const char*
gparts_units_find_format(double value)
{
    const GPartsUnitsFormat *format = &gparts_units_format[0];

    while (fabs(value) < format->value) {
        format++;
    }

    return format->string;
}

/*! \brief Finds the right metric prefix for a given value.
 *
 *  \param [in] value The value in base units.
 *  \return A pointer to a GPartsUnitsMetricPrefix.  This function does not
 *  return NULL.
 */
static const GPartsUnitsMetricPrefix*
gparts_units_find_metric_prefix(double value)
{
    const GPartsUnitsMetricPrefix *prefix = &gparts_units_metric_prefixes[0];

    if (value == 0) {
        value = 1;
    }

    while (fabs(value) < prefix->value) {
        prefix++;
    }

    return prefix;
}

GType
gparts_units_get_type(void)
{
  static GType type = G_TYPE_INVALID;

  if (type == G_TYPE_INVALID) {

    type = g_boxed_type_register_static( "GPartsUnits",
                                         (GBoxedCopyFunc) gparts_units_copy,
                                         (GBoxedFreeFunc) gparts_units_free
    );

    g_value_register_transform_func(GPARTS_TYPE_UNITS,
                                    G_TYPE_STRING,
                                    gparts_units_transform);
  }

  return type;
}

void
gparts_units_free(GPartsUnits *units)
{
    g_free(units);
}

GPartsUnitsNewFunc
gparts_units_get_new_func(const char *name)
{
    struct entry {
        const char        *name;
        GPartsUnitsNewFunc func;
    };

    static const struct entry table[] =
    {
        { "Capacitance", gparts_units_new_farads  },
        { "PD",          gparts_units_new_watts   },
        { "IC",          gparts_units_new_amps    },
        { "ID",          gparts_units_new_amps    },
        { "FT",          gparts_units_new_hertz   },
        { "Frequency",   gparts_units_new_hertz   },
        { "Inductance",  gparts_units_new_henrys  },
        { "IF",          gparts_units_new_amps    },
        { "Resistance",  gparts_units_new_ohms    },
        { "Tolerance",   gparts_units_new_pp      },
        { "Stability",   gparts_units_new_pp      },
        { "VBR",         gparts_units_new_volts   },
        { "VGS",         gparts_units_new_volts   },
        { "VDSS",        gparts_units_new_volts   },
        { "VZ",          gparts_units_new_volts   },
        { "VF",          gparts_units_new_volts   },
        { "VR",          gparts_units_new_volts   },
        { "VRWM",        gparts_units_new_volts   },
        { "VZ",          gparts_units_new_volts   },
        { "MinTemp",     gparts_units_new_celcius },
        { "MaxTemp",     gparts_units_new_celcius },
        { NULL,          gparts_units_new_none    }
    };

    GPartsUnitsNewFunc func = NULL;
    const struct entry *t = &table[0];

    while (func == NULL) {

        if ((t->name == NULL) || (g_strcmp0(t->name, name) == 0)) {
            func = t->func;
        }
        else {
            t++;
        }
    }

    return func;
}

GPartsUnits*
gparts_units_new_amphours(double amphours)
{
    GPartsUnits *units = GPARTS_UNITS(g_malloc(sizeof(GPartsUnits)));

    units->value  = amphours;
    units->func   = gparts_units_transform_std;
    units->symbol = "Ah";

    return units;
}

GPartsUnits*
gparts_units_new_amps(double amps)
{
    GPartsUnits *units = GPARTS_UNITS(g_malloc(sizeof(GPartsUnits)));

    units->value  = amps;
    units->func   = gparts_units_transform_std;
    units->symbol = "A";

    return units;
}

GPartsUnits*
gparts_units_new_celcius(double celcius)
{
    GPartsUnits *units = GPARTS_UNITS(g_malloc(sizeof(GPartsUnits)));

    units->value  = celcius;
    units->func   = gparts_units_transform_std;
    units->symbol = GPARTS_SYMBOL_CELCIUS;

    return units;
}

GPartsUnits*
gparts_units_new_farads(double farads)
{
    GPartsUnits *units = GPARTS_UNITS(g_malloc(sizeof(GPartsUnits)));

    units->value  = farads;
    units->func   = gparts_units_transform_std;
    units->symbol = "F";

    return units;
}

GPartsUnits*
gparts_units_new_grams(double grams)
{
    GPartsUnits *units = GPARTS_UNITS(g_malloc(sizeof(GPartsUnits)));

    units->value  = grams;
    units->func   = gparts_units_transform_std;
    units->symbol = "g";

    return units;
}

GPartsUnits*
gparts_units_new_henrys(double henrys)
{
    GPartsUnits *units = GPARTS_UNITS(g_malloc(sizeof(GPartsUnits)));

    units->value  = henrys;
    units->func   = gparts_units_transform_std;
    units->symbol = "H";

    return units;
}

GPartsUnits*
gparts_units_new_hertz(double hertz)
{
    GPartsUnits *units = GPARTS_UNITS(g_malloc(sizeof(GPartsUnits)));

    units->value  = hertz;
    units->func   = gparts_units_transform_std;
    units->symbol = "Hz";

    return units;
}

GPartsUnits*
gparts_units_new_meters(double meters)
{
    GPartsUnits *units = GPARTS_UNITS(g_malloc(sizeof(GPartsUnits)));

    units->value  = meters;
    units->func   = gparts_units_transform_std;
    units->symbol = "m";

    return units;
}

GPartsUnits*
gparts_units_new_none(double value)
{
    GPartsUnits *units = GPARTS_UNITS(g_malloc(sizeof(GPartsUnits)));

    units->value  = value;
    units->func   = gparts_units_transform_std;
    units->symbol = "";

    return units;
}

GPartsUnits*
gparts_units_new_ohms(double ohms)
{
    GPartsUnits *units = GPARTS_UNITS(g_malloc(sizeof(GPartsUnits)));

    units->value  = ohms;
    units->func   = gparts_units_transform_std;
    units->symbol = GPARTS_SYMBOL_OHM;

    return units;
}

GPartsUnits*
gparts_units_new_percent(double percent)
{
    GPartsUnits *units = GPARTS_UNITS(g_malloc(sizeof(GPartsUnits)));

    units->value  = percent;
    units->func   = gparts_units_transform_percent;
    units->symbol = "%";

    return units;
}

GPartsUnits*
gparts_units_new_pp(double pp)
{
    GPartsUnits *units = GPARTS_UNITS(g_malloc(sizeof(GPartsUnits)));

    units->value  = pp;
    units->func   = gparts_units_transform_pp;
    units->symbol = NULL;

    return units;
}

GPartsUnits*
gparts_units_new_volts(double volts)
{
    GPartsUnits *units = GPARTS_UNITS(g_malloc(sizeof(GPartsUnits)));

    units->value  = volts;
    units->func   = gparts_units_transform_std;
    units->symbol = "V";

    return units;
}

GPartsUnits*
gparts_units_new_watts(double watts)
{
    GPartsUnits *units = GPARTS_UNITS(g_malloc(sizeof(GPartsUnits)));

    units->value  = watts;
    units->func   = gparts_units_transform_std;
    units->symbol = "W";

    return units;
}


static void
gparts_units_transform(const GValue *src_value, GValue *dest_value)
{
    if (G_IS_VALUE(src_value)) {

        GPartsUnits *units = g_value_get_boxed(src_value);

        units->func(src_value, dest_value);
    }
}

static void
gparts_units_transform_percent(const GValue *src_value, GValue *dest_value)
{
    if (G_IS_VALUE(src_value))
    {
        GPartsUnits *units = g_value_get_boxed(src_value);
        GString *string = g_string_new("");
        double percent = 100.0 * units->value;
        int digits = 0;
        const char *symbol = units->symbol;

        if (symbol == NULL) {
            symbol = "%";
        }

        if (percent != 0) {

            digits = ceil(-log10(fabs(percent)));

            if (digits < 0)
            {
                digits = 0;
            }
        }

        g_string_printf(string, "%.*f %s", digits, percent, symbol);

        g_value_take_string(dest_value, string->str);
        g_string_free(string, FALSE);
    }
    else {
        g_value_set_string(dest_value, "Error");
    }
}

static void
gparts_units_transform_pp(const GValue *src_value, GValue *dest_value)
{
    if (G_IS_VALUE(src_value)) {

        GPartsUnits *units = g_value_get_boxed(src_value);
        GString *string = g_string_new("");
        double pp = units->value;
        int digits = 0;
        const char *symbol;

        if (fabs(pp) >= 1e-3) {
            pp *= 100;
            symbol = "%";
        }
        else if (fabs(pp) >= 1e-6) {
            pp *= 1e6;
            symbol = "ppm";
        }
        else if (fabs(pp) >= 1e-9) {
            pp *= 1e9;
            symbol = "ppb";
        }
        else if (fabs(pp) >= 1e-12) {
            pp *= 1e12;
            symbol = "ppt";
        }
        else {
            pp *= 1e15;
            symbol = "ppq";
        }

        if (pp != 0) {

            digits = ceil(-log10(fabs(pp)));

            if (digits < 0) {
                digits = 0;
            }
        }

        g_string_printf(string, "%.*f %s", digits, pp, symbol);

        g_value_take_string(dest_value, string->str);
        g_string_free(string, FALSE);
    }
    else {
        g_value_set_string(dest_value, "Error");
    }
}

static void
gparts_units_transform_std(const GValue *src_value, GValue *dest_value)
{
    if (G_IS_VALUE(src_value)) {

        GPartsUnits *units = g_value_get_boxed(src_value);
        GString *string = g_string_new("");
        double n = units->value;
        const GPartsUnitsMetricPrefix* prefix = gparts_units_find_metric_prefix(n);

        const char *format = gparts_units_find_format(n/prefix->value);
        const char *symbol = units->symbol;

        if (symbol == NULL) {
            symbol = "";
        }

        g_string_printf(string, format, n/prefix->value, prefix->symbol, symbol);

        g_value_take_string(dest_value, string->str);
        g_string_free(string, FALSE);
    }
    else {
        g_value_set_string(dest_value, "Error");
    }
}


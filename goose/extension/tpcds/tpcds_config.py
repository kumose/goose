import os

# list all include directories
include_directories = [
    os.path.sep.join(x.split('/'))
    for x in ['extension/tpcds/include', 'extension/tpcds/dsdgen/include', 'extension/tpcds/dsdgen/include/dsdgen-c']
]
# source files
source_files = [os.path.sep.join(x.split('/')) for x in ['extension/tpcds/tpcds_extension.cc']]
source_files += [
    os.path.sep.join(x.split('/'))
    for x in [
        'extension/tpcds/dsdgen/dsdgen.cc',
        'extension/tpcds/dsdgen/append_info-c.cc',
        'extension/tpcds/dsdgen/dsdgen_helpers.cc',
    ]
]
source_files += [
    os.path.sep.join(x.split('/'))
    for x in [
        'extension/tpcds/dsdgen/dsdgen-c/skip_days.cc',
        'extension/tpcds/dsdgen/dsdgen-c/address.cc',
        'extension/tpcds/dsdgen/dsdgen-c/build_support.cc',
        'extension/tpcds/dsdgen/dsdgen-c/date.cc',
        'extension/tpcds/dsdgen/dsdgen-c/dbgen_version.cc',
        'extension/tpcds/dsdgen/dsdgen-c/decimal.cc',
        'extension/tpcds/dsdgen/dsdgen-c/dist.cc',
        'extension/tpcds/dsdgen/dsdgen-c/error_msg.cc',
        'extension/tpcds/dsdgen/dsdgen-c/genrand.cc',
        'extension/tpcds/dsdgen/dsdgen-c/join.cc',
        'extension/tpcds/dsdgen/dsdgen-c/list.cc',
        'extension/tpcds/dsdgen/dsdgen-c/load.cc',
        'extension/tpcds/dsdgen/dsdgen-c/misc.cc',
        'extension/tpcds/dsdgen/dsdgen-c/nulls.cc',
        'extension/tpcds/dsdgen/dsdgen-c/parallel.cc',
        'extension/tpcds/dsdgen/dsdgen-c/permute.cc',
        'extension/tpcds/dsdgen/dsdgen-c/pricing.cc',
        'extension/tpcds/dsdgen/dsdgen-c/r_params.cc',
        'extension/tpcds/dsdgen/dsdgen-c/release.cc',
        'extension/tpcds/dsdgen/dsdgen-c/scaling.cc',
        'extension/tpcds/dsdgen/dsdgen-c/scd.cc',
        'extension/tpcds/dsdgen/dsdgen-c/sparse.cc',
        'extension/tpcds/dsdgen/dsdgen-c/StringBuffer.cc',
        'extension/tpcds/dsdgen/dsdgen-c/tdef_functions.cc',
        'extension/tpcds/dsdgen/dsdgen-c/tdefs.cc',
        'extension/tpcds/dsdgen/dsdgen-c/text.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_call_center.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_catalog_page.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_catalog_returns.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_catalog_sales.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_customer.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_customer_address.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_customer_demographics.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_datetbl.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_household_demographics.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_income_band.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_inventory.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_item.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_promotion.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_reason.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_ship_mode.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_store.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_store_returns.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_store_sales.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_timetbl.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_warehouse.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_web_page.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_web_returns.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_web_sales.cc',
        'extension/tpcds/dsdgen/dsdgen-c/w_web_site.cc',
        'extension/tpcds/dsdgen/dsdgen-c/init.cc',
    ]
]

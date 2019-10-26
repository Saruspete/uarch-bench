/*
 * default_benches.cpp
 *
 * Various "default" benchmarks.
 */

#include "benchmark.hpp"
#include "util.hpp"
#include "boost/preprocessor/repetition/repeat_from_to.hpp"

extern "C" {

bench2_f rs_dep_add;
bench2_f rs_dep_add4;
bench2_f rs_dep_imul;
bench2_f rs_split_stores;
bench2_f rs_dep_fsqrt;

#define MAX_RATIO 10
#define DECL_MANY(fname, op) BOOST_PP_REPEAT_FROM_TO(0, MAX_RATIO, DECL_BENCH2, fname ## op)

DECL_MANY(rs_fsqrt_,nop)
DECL_MANY(rs_fsqrt_,add)
DECL_MANY(rs_fsqrt_,xorzero)
DECL_MANY(rs_fsqrt_,load)
DECL_MANY(rs_fsqrt_,store)
DECL_MANY(rs_fsqrt_,paddb)
DECL_MANY(rs_fsqrt_,vpaddb)
DECL_MANY(rs_fsqrt_,add_padd)
DECL_MANY(rs_fsqrt_,load_dep)

DECL_MANY(rs_load_,nop)
DECL_MANY(rs_load_,add)

BOOST_PP_REPEAT_FROM_TO(0, 80, DECL_BENCH2, rs_loadchain)

BOOST_PP_REPEAT_FROM_TO(0, 80, DECL_BENCH2, rs_storebuf)

}


template <typename TIMER>
void register_rstalls(GroupList& list) {
#if !UARCH_BENCH_PORTABLE

    std::shared_ptr<BenchmarkGroup> group = std::make_shared<BenchmarkGroup>("studies/resource-stalls", "Test RESOURCE_STALLS events");
    list.push_back(group);

    auto maker = DeltaMaker<TIMER>(group.get(), 1000);

    maker.template make<rs_dep_add     >("dep-add" ,    "Dependent adds (RS limit)",  128);
    maker.template make<rs_dep_add4    >("dep-add4",    "Inependent adds (? limit)",  128);
    maker.template make<rs_dep_imul    >("dep-imul",    "Dependent imuls (RS limit)", 128);
    maker.template make<rs_split_stores>("split-store", "Split stores (SB limit)",    128);
    maker.template make<rs_dep_fsqrt>   ("fsqrt",       "Dependent fqrt (RS limit)",  128);

    // a macro to call maker.make on test that mix fsqrt and antoher op in a variery of ratios
#define MAKE_FSQRT_OP(z, n, op) maker.template make<rs_fsqrt_ ## op ## n>("fsqrt-" #op "-" #n, \
        "fsqrt:" #op " in 1:" #n " ratio", 32);

    BOOST_PP_REPEAT_FROM_TO(0, MAX_RATIO, MAKE_FSQRT_OP, nop)
    BOOST_PP_REPEAT_FROM_TO(0, MAX_RATIO, MAKE_FSQRT_OP, add)
    BOOST_PP_REPEAT_FROM_TO(0, MAX_RATIO, MAKE_FSQRT_OP, xorzero)
    BOOST_PP_REPEAT_FROM_TO(0, MAX_RATIO, MAKE_FSQRT_OP, load)
    BOOST_PP_REPEAT_FROM_TO(0, MAX_RATIO, MAKE_FSQRT_OP, store)
    BOOST_PP_REPEAT_FROM_TO(0, MAX_RATIO, MAKE_FSQRT_OP, paddb)
    BOOST_PP_REPEAT_FROM_TO(0, MAX_RATIO, MAKE_FSQRT_OP, vpaddb)
    BOOST_PP_REPEAT_FROM_TO(0, MAX_RATIO, MAKE_FSQRT_OP, add_padd)
    BOOST_PP_REPEAT_FROM_TO(0, MAX_RATIO, MAKE_FSQRT_OP, load_dep)

#define MAKE_LOAD_OP(z, n, op) maker.template make<rs_load_ ## op ## n>("load-" #op "-" #n, \
        "load:" #op " in 1:" #n " ratio", 32);

    BOOST_PP_REPEAT_FROM_TO(0, MAX_RATIO, MAKE_LOAD_OP, nop)
    BOOST_PP_REPEAT_FROM_TO(0, MAX_RATIO, MAKE_LOAD_OP, add)

#define MAKE_LOADCHAIN(z, n, _) maker.template make<rs_loadchain ## n>("loadchain-" #n, \
        "loadchain: " #n " loads", 32);

    BOOST_PP_REPEAT_FROM_TO(0, 80, MAKE_LOADCHAIN, _)

#define MAKE_STOREBUF(z, n, _) maker.template make<rs_storebuf ## n>("storebuf-" #n, \
        "storebuf: " #n " stores", 32);

    BOOST_PP_REPEAT_FROM_TO(0, 80, MAKE_STOREBUF, _)


#endif // #if !UARCH_BENCH_PORTABLE

}

#define REG_THIS(CLOCK) template void register_rstalls<CLOCK>(GroupList& list);

ALL_TIMERS_X(REG_THIS)




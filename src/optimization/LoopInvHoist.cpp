#include <algorithm>
#include "logging.hpp"
#include "LoopSearch.hpp"
#include "LoopInvHoist.hpp"
#define inset(set,operand) (set.find(operand)!=set.end())
using namespace std;

void LoopInvHoist::run()
{
    // 先通过LoopSearch获取循环的相关信�?
    LoopSearch loop_searcher(m_, false);
    loop_searcher.run();

    // 接下来由你来补充啦！
    auto func_lists=m_->get_functions();
    for(auto func: func_lists){
        if(func->get_basic_blocks().size()==0){
            continue;
        }
        else{
            auto loops=loop_searcher.get_loops_in_func(func);
            for(auto each_loop:loops){
                auto loop_base=loop_searcher.get_loop_base(each_loop);
                auto now_loop=loop_searcher.get_inner_loop(loop_base);
                set<Value*> used_ins_op;
                vector<Instruction*> del;
                while(now_loop){
                    for(BasicBlock* bb:*now_loop){
                        for(Instruction* ins: bb->get_instructions()){
                            if(ins->is_phi())
                                used_ins_op.insert(ins);
                        }
                    }

                    for(BasicBlock* bb:*now_loop){
                        for(Instruction* instr: bb->get_instructions()){
                            if(instr->isBinary()){
                                auto ins_1=instr->get_operand(0);
                                auto ins_2=instr->get_operand(1);
                                if(!inset(used_ins_op,ins_1)&&!inset(used_ins_op,ins_2)){
                                    del.push_back(instr);
                                }
                                else{
                                    used_ins_op.insert(instr);
                                }
                            }
                            else if(instr->is_cmp()||instr->is_fcmp()){
                                auto ins_1=instr->get_operand(0);
                                auto ins_2=instr->get_operand(1);
                                if(!inset(used_ins_op,ins_1)&&!inset(used_ins_op,ins_2)){
                                    del.push_back(instr);
                                }
                                else{
                                    used_ins_op.insert(instr);
                                }
                            }
                            else if(instr->is_zext()){
                                auto ins_1=instr->get_operand(0);
                                if(!inset(used_ins_op,ins_1)){
                                    del.push_back(instr);
                                }
                                else{
                                    used_ins_op.insert(instr);
                                }
                            }
                            else if(instr->is_si2fp()||instr->is_fp2si()){
                                auto ins_1=instr->get_operand(0);
                                if(!inset(used_ins_op,ins_1)){
                                    del.push_back(instr);
                                }
                                else{
                                    used_ins_op.insert(instr);
                                }
                            }
                        }
                        for(auto ins_cash:del){
                            bb->delete_instr(ins_cash);
                        }
                    }
                    auto base=loop_searcher.get_loop_base(now_loop);
                    while(!del.empty()){
                        BasicBlock* pre_bb;
                        for(auto pre_BB:base->get_pre_basic_blocks()){
                            if (!inset((*now_loop),pre_BB)){
                                pre_bb=pre_BB;
                                break;
                            }
                        }
                        pre_bb->add_instr_begin(del.back());
                        del.pop_back();
                    }
                    now_loop=loop_searcher.get_parent_loop(now_loop);
                    del.clear();
                    used_ins_op.clear();
                }
            }
        }
    }
}

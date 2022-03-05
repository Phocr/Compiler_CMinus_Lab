#include "ActiveVars.hpp"
using namespace std;
#define notcons(operand) (!cast_constantint(operand)&&!cast_constantfp(operand))
#define inset(set,operand) (set.find(operand)!=set.end())

void ActiveVars::run()
{
    std::ofstream output_active_vars;
    output_active_vars.open("active_vars.json", std::ios::out);
    output_active_vars << "[";
    for (auto &func : this->m_->get_functions()) {
        if (func->get_basic_blocks().empty()) {
            continue;
        }
        else
        {
            func_ = func;  

            func_->set_instr_name();
            live_in.clear();
            live_out.clear();
            
            // 在此分析 func_ 的每个bb块的活跃变量，并存储在 live_in live_out 结构内
            int change = 1;
            std::map<BasicBlock *, std::set<Value *>> use,def;
            for (auto bb : func_->get_basic_blocks())
            {
                use[bb].clear();
                def[bb].clear();
                for(auto ins: bb->get_instructions())
                {
                    if(ins->isBinary() || ins->is_cmp() || ins->is_fcmp()){
                        auto left = ins->get_operand(0);
                        auto right = ins->get_operand(1);
                        if(!inset(def[bb],left)&&notcons(left)){
                            use[bb].insert(left);
                        }
                        if(!inset(def[bb],right)&&notcons(right)){
                            use[bb].insert(right);
                        }
                        if(!inset(use[bb],ins)){
                            def[bb].insert(ins);
                        }
                    }
                    else if(ins->is_br()){
                        int ops=ins->get_num_operand();
                        if(ops==1) continue;
                        else{
                            auto cond = ins->get_operand(0);
                            if(!inset(def[bb],cond)&&notcons(cond)){
                                use[bb].insert(cond);
                            }
                        }
                    }
                    else if(ins->is_call()){
                        int ops=ins->get_num_operand();
                        int i=1;
                        if(ops!=1)
                        {
                            i=1;
                            while(i<ops){
                                auto arg = ins->get_operand(i);
                                if(!inset(def[bb],arg)&&notcons(arg)){
                                    use[bb].insert(arg);
                                }
                                i++;
                            }
                        }
                        auto callInstr=dynamic_cast<CallInst*>(ins);
                        if (callInstr!=nullptr){
                            if(!callInstr->get_function_type()->is_void_type()){
                                if(!inset(use[bb],ins)){
                                    def[bb].insert(ins);
                                }
                            }
                        }
                        else
                        {
                            printf("error");
                        }
                    }
                    else if(ins->is_fp2si()||ins->is_si2fp()){
                        int ops=ins->get_num_operand();
                        if(ops==0) printf("error");
                        else{
                            auto operand = ins->get_operand(0);
                            if(ins->is_fp2si()){
                                if(!inset(def[bb],operand)&&!cast_constantfp(operand)){
                                    use[bb].insert(operand);
                                }
                            }
                            else{
                                if(!inset(def[bb],operand)&&!cast_constantint(operand)){
                                    use[bb].insert(operand);
                                }
                            }
                            if(!inset(use[bb],ins)){
                                def[bb].insert(ins);
                            }
                        }
                    }
                    else if(ins->is_zext()||ins->is_load()){
                        int ops=ins->get_num_operand();
                        if(ops==0) printf("error");
                        else{
                            auto operand = ins->get_operand(0);
                            if(!inset(def[bb],operand)&&notcons(operand)){
                                use[bb].insert(operand);
                            }
                            if(!inset(use[bb],ins)){
                                def[bb].insert(ins);
                            }
                        }
                    }
                    else if(ins->is_phi()){
                        auto phi =  dynamic_cast<PhiInst*>(ins);
                        int ops=ins->get_num_operand();
                        if(phi!=nullptr&&ops!=0){
                            for(int i = 0; i < ops/2; i++){
                                auto operand = phi->get_operand(2*i);
                                if(!inset(def[bb],operand)&&notcons(operand)){
                                    use[bb].insert(operand);
                                }
                            }
                            if(!inset(use[bb],ins)){
                                def[bb].insert(ins);
                            }
                        }
                        else{
                            printf("error");
                        }
                    }
                    else if(ins->is_gep()){
                        auto gep =  dynamic_cast<GetElementPtrInst*>(ins);
                        int ops=ins->get_num_operand();
                        if(gep!=nullptr&&ops!=0){
                            for(int i = 0; i < ops; i++){
                                auto operand = gep->get_operand(i);
                                if(!inset(def[bb],operand)&&notcons(operand)){
                                    use[bb].insert(operand);
                                }
                            }
                            if(!inset(use[bb],ins)){
                                def[bb].insert(ins);
                            }
                        } 
                        else{
                            printf("error");
                        }
                    }
                    else if(ins->is_alloca()){
                        if(!inset(use[bb],ins)){
                            def[bb].insert(ins);
                        }
                    }
                    else if(ins->is_store()){
                        int ops=ins->get_num_operand();
                        if(ops<2) printf("error");
                        else{
                            auto left = ins->get_operand(0);
                            auto right = ins->get_operand(1);
                            if(!inset(def[bb],left)&&notcons(left)){
                                use[bb].insert(left);
                            }
                            if(!inset(def[bb],right)&&notcons(right)){
                                use[bb].insert(right);
                            }
                        }
                    }
                }
            }
            set<Value *> out,in,del;
            while (change) {
                change = 0;
                for (auto bb : func->get_basic_blocks())
                {
                    out.clear();
                    in.clear();
                    for(auto a:live_in[bb]){
                        in.insert(a);
                    }
                    for(auto succ : bb->get_succ_basic_blocks())
                    {
                        int i;
                        for(auto a:live_in[succ]){//OUT[B]=U IN[s]
                            out.insert(a);
                        }
                        del.clear();
                        for(auto phi_ins : succ->get_instructions()){
                            if(!phi_ins->is_phi()){
                                 break;
                            }
                            for(i = 0; i < phi_ins->get_num_operand()/2; i++){
                                if(phi_ins->get_operand(2*i+1) != bb)
                                    del.insert(phi_ins->get_operand(2*i));
                            }
                            for(auto dele : del){ 
                                if(inset(out,dele))
                                    out.erase(dele);
                            }
                        }
                    }
                    live_out[bb].clear();
                    for(auto a:out){
                        live_out[bb].insert(a);
                    }
                    for(auto a : def[bb]){//OUT[B]-def
                        if(inset(out,a))
                            out.erase(a);
                    }
                    for(auto a:use[bb]){//useU(OUT[B]-def)
                        out.insert(a);
                    }
                    live_in[bb].clear();
                    for(auto a:out){
                        live_in[bb].insert(a);
                    }
                    if(in != live_in[bb])
                        change = 1;
                }
            }

            output_active_vars << print();
            output_active_vars << ",";
        }
    }
    output_active_vars << "]";
    output_active_vars.close();
    return ;
}

std::string ActiveVars::print()
{
    std::string active_vars;
    active_vars +=  "{\n";
    active_vars +=  "\"function\": \"";
    active_vars +=  func_->get_name();
    active_vars +=  "\",\n";

    active_vars +=  "\"live_in\": {\n";
    for (auto &p : live_in) {
        if (p.second.size() == 0) {
            continue;
        } else {
            active_vars +=  "  \"";
            active_vars +=  p.first->get_name();
            active_vars +=  "\": [" ;
            for (auto &v : p.second) {
                active_vars +=  "\"%";
                active_vars +=  v->get_name();
                active_vars +=  "\",";
            }
            active_vars += "]" ;
            active_vars += ",\n";   
        }
    }
    active_vars += "\n";
    active_vars +=  "    },\n";
    
    active_vars +=  "\"live_out\": {\n";
    for (auto &p : live_out) {
        if (p.second.size() == 0) {
            continue;
        } else {
            active_vars +=  "  \"";
            active_vars +=  p.first->get_name();
            active_vars +=  "\": [" ;
            for (auto &v : p.second) {
                active_vars +=  "\"%";
                active_vars +=  v->get_name();
                active_vars +=  "\",";
            }
            active_vars += "]";
            active_vars += ",\n";
        }
    }
    active_vars += "\n";
    active_vars += "    }\n";

    active_vars += "}\n";
    active_vars += "\n";
    return active_vars;
}
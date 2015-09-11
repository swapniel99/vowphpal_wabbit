#include <phpcpp.h>
#include <vwdll.h>

/**
 *  tell the compiler that the get_module is a pure C function
 */

class Vowpal_Wabbit : public Php::Base
{
    private:
        static void* _modelPointer;
    public:
        Vowpal_Wabbit() {}
        virtual ~Vowpal_Wabbit() {}
        static Php::Value getPrediction(Php::Parameters &params)
        {
            return 0;
        }
};

void* Vowpal_Wabbit::_modelPointer = NULL; //VW_InitializeA("-t -i model.vw");

extern "C" {
    
    /**
     *  Function that is called by PHP right after the PHP process
     *  has started, and that returns an address of an internal PHP
     *  strucure with all the details and features of your extension
     *
     *  @return void*   a pointer to an address that is understood by PHP
     */
    PHPCPP_EXPORT void *get_module() 
    {
        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension vwextension("vowphpal_wabbit", "1.0");
        
        // @todo    add your own functions, classes, namespaces to the extension
        Php::Class<Vowpal_Wabbit> vowpal_wabbit("Vowpal_Wabbit");
        vowpal_wabbit.method("getPrediction", &Vowpal_Wabbit::getPrediction);

        vwextension.add(std::move(vowpal_wabbit));
        
        // return the extension
        return vwextension;
    }
}

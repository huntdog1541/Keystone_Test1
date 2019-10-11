#include <stdio.h>


#include <stdio.h>
#include <string.h>
#include <keystone/keystone.h>

static int test_ks(ks_arch arch, int mode, const char * assembly, int syntax)
{
    ks_engine *ks;
    ks_err err;
    size_t count;
    unsigned char *encode;
    size_t size;

    err = ks_open(arch, mode, &ks);
    if(err != KS_ERR_OK)
    {
        printf("ERROR: failed on ks_open(), quit\n");
        return -1;
    }

    if(syntax)
        ks_option(ks, KS_OPT_SYNTAX, syntax);

    if(ks_asm(ks, assembly, 0, &encode, &size, &count))
    {
        printf("ERROR: failed on ks_asm() with count = %lu, error code = %u, with %s \n", count, ks_error(ks), assembly);
    }
    else
    {
        size_t i;
        printf("%s = ", assembly);
        for(i = 0; i < size; i++)
        {
            printf("%02x ", encode[i]);
        }
        printf("\n");
        printf("Assembled: %lu bytes, %lu statements\n\n", size, count);
    }

    // NOTE: free encode after usage to avoid leaking memory
    ks_free(encode);
    // close Keystone instance when done
    ks_close(ks);
}

int main(int argc, char ** argv)
{
    test_ks(KS_ARCH_X86, KS_MODE_16, "add eax, ecx", 0);

    test_ks(KS_ARCH_X86, KS_MODE_16, "add eax, 15h", 0);

    printf("Printing program ATT_SYNTAX\n");
    test_ks(KS_ARCH_X86, KS_MODE_64, "push %rbp", KS_OPT_SYNTAX_ATT);
    test_ks(KS_ARCH_X86, KS_MODE_64, "mov %rbp, %rsp", KS_OPT_SYNTAX_ATT);
    test_ks(KS_ARCH_X86, KS_MODE_64, "mov %edi, -0x4(%rbp)", KS_OPT_SYNTAX_ATT);
    test_ks(KS_ARCH_X86, KS_MODE_64, "mov -0x4(%rbp), %eax", KS_OPT_SYNTAX_ATT);
    test_ks(KS_ARCH_X86, KS_MODE_64, "imul %eax, %eax", KS_OPT_SYNTAX_ATT);

    printf("Printing program INTEL_SYNTAX\n");
    test_ks(KS_ARCH_X86, KS_MODE_64, "push rbp", KS_OPT_SYNTAX_INTEL);
    test_ks(KS_ARCH_X86, KS_MODE_64, "mov rbp, rsp", KS_OPT_SYNTAX_INTEL);
    test_ks(KS_ARCH_X86, KS_MODE_64, "mov DWORD PTR [rbp-0x4], edi", KS_OPT_SYNTAX_INTEL);
    test_ks(KS_ARCH_X86, KS_MODE_64, "mov eax, DWORD PTR [rbp-0x4]", KS_OPT_SYNTAX_INTEL);
    test_ks(KS_ARCH_X86, KS_MODE_64, "imul eax, eax", KS_OPT_SYNTAX_INTEL);

    return 0;
}
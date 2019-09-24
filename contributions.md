## Contributions

Any and all contributions are welcome! 

## Style Guide

This project is written using c-style c++ with templates and namespaces.

- Prefer c headers
- Manual memory management, avoid malloc for small allocations, use LinearAllocator and PoolAllocator
- Use free and malloc, not new and delete
- Use free functions unless absolutely necessary eg operator overloading
- Avoid excessive use of templates
- Don't use inheritance, virtual functions or Constructors (Conversion Constructor's are okay)
- Put everything into the top namespace
- Default initialize struct to 0


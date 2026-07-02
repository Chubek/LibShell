set(LIBSH_DESIGN_DOCS
    design/architecture.md
    design/dsl.md
    design/kernels.md
    design/puppeteer.md
    design/sinklets.md
    design/xi.md
)

add_custom_target(libsh-docs SOURCES ${LIBSH_DESIGN_DOCS})

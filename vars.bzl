def _defineProgrammer_impl(ctx):
  return [
    platform_common.TemplateVariableInfo({
      "PROGPORT": ctx.attr.port
    })
  ]

defineProgrammer = rule(
  implementation = _defineProgrammer_impl,
  attrs = { "port": attr.string() }
)
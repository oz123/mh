###
#  general configuration of the makefile itself
###
SHELL := /bin/bash
.DEFAULT_GOAL := help

.PHONY: test

PY ?= python3

define PRINT_HELP_PYSCRIPT
# BSD 2-Clause License
#
# Copyright (c) 2021, Oz N Tiram <oz.tiram@gmail.com>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
import re, sys
from collections import defaultdict

targets = {}
variables = []
local_variables = defaultdict(list)

print("Targets:\n")
for line in sys.stdin:
    match = re.search(r'^([a-zA-Z_-]+):.*?## (.*)$$', line)
    if match:
        target, help = match.groups()
        targets.update({target: help})
    local_var = re.search(r'^(?P<target>[\w]+):.(?P<var>[A-Z_]+).*#\? (?P<help>.*)$$', line)
    if local_var:
        gd = local_var.groupdict()
        local_variables[gd['target']].append({'name': gd['var'], 'help': gd['help']})
        continue
    vars = re.search(r'([A-Z_]+).*#\? (.*)$$', line)
    if vars:
        target, help = vars.groups()
        variables.append("%-20s %s" % (target, help))

for target, help in targets.items():
    print("%-20s %s\n" % (target, help))
    if target in local_variables:
        [print("  %s - %s " % (i['name'], i['help'])) for i in local_variables[target]]
    print("")
if variables:
    print("Global Variables you can override:\n")
    print('\n'.join(variables))

endef

export PRINT_HELP_PYSCRIPT

VERSION ?= $(shell git describe --always) #? version
REGISTRY ?= docker.io #? container registry to push
ORG ?= oz123 #? organization to push to
IMG ?= $(CWD) #? image name
OPTS ?= #? add extra OPTS to misc commands


help:
	@$(PY) -c "$$PRINT_HELP_PYSCRIPT" < $(MAKEFILE_LIST)

test: HOST ?= localhost #? what host is running NGinx
test: COLOR ?= green #? which color should be the backround
test: ## test that the HOST shows background COLOR
	curl -k -L $(HOST) | grep -qc "background: $(COLOR)"

docker-build::  ## build a docker image
	docker build $(OPTS) -t $(REGISTRY)/$(ORG)/$(IMG):v$(TAG) -f Dockerfile .

docker-push::  ## push docker image to $(REGISTRY)
	docker push $(REGISTRY)/$(ORG)/$(IMG):v$(TAG)


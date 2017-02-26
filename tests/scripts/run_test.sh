#!/bin/bash

pushd ../.. > /dev/null 2>&1

/usr/bin/time --format="Test input: %C\nDuraction in seconds: %e\n" $@

popd > /dev/null 2>&1

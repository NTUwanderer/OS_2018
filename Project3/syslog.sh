#!/bin/bash
{
	echo "<2> $@" | sudo tee /dev/kmsg
} &> /dev/null


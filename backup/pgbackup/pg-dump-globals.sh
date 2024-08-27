#!/bin/bash

# Dump globals - accounts included
pg_dumpall --globals-only > globals.sql

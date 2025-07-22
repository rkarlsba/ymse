#!/usr/bin/env python3
# vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

# -*- coding: utf-8 -*-

import ldap
import logging
import re
import sys
import traceback
from ldap.controls import SimplePagedResultsControl
import ldapstuff

# Globals
debug = 1

# This one returns the first token in the format username:password
def getpwtok():
    try:
        f=open(ldap_pw_file,"r")
        lines=f.readlines()
        if (len(lines) > 1):
            print("More than one line in {ldap_pw_file}, only one considered", file=sys.stderr)
        for line in lines:
            line = re.sub(r"[\r\n]", "", line)
            auth_token = line.split(':')
            break
        f.close()
        return auth_token

    except Exception as e:
        logging.error(traceback.format_exc())

def query_activedirectory(uri, bindDN, bindPW, baseDN, filterstr='(objectClass=*)', attrlist=None, timeout=-1, pagesize=250, decodeBytes=True):

    con = ldap.initialize(uri, bytes_mode=False)
    con.protocol_version = ldap.VERSION3
    con.set_option(ldap.OPT_REFERRALS,0)  # required for AD authentication
    con.simple_bind_s(bindDN, bindPW)
    
    # optional, but reduce the number of supported control, since only this one will be parsed
    known_ldap_resp_ctrls = {
        SimplePagedResultsControl.controlType: SimplePagedResultsControl,
    }
    
    # instantiate the control that will make the paged results
    # it carries the page cookie (initially empty, to request the first page)
    req_ctrl = SimplePagedResultsControl(
        criticality=True,
        size=pagesize,
        cookie=''
    )
    
    while True:
        # query next page, asynchronous
        msgid = con.search_ext(
            baseDN,
            ldap.SCOPE_SUBTREE,
            filterstr,
            attrlist=attrlist,
            serverctrls=[req_ctrl],
        )
        
        # get the page results
        rtype, rdata, rmsgid, serverctrls = con.result3(
            msgid,
            timeout=timeout,
            resp_ctrl_classes=known_ldap_resp_ctrls,
        )
        
        # just assure the rtype is ok
        # we don't really expect other result, but RES_SEARCH_RESULT
        if rtype == ldap.RES_SEARCH_RESULT:
            # process results
            for dn, attrs in rdata:
                if dn:
                    if decodeBytes:
                        attrs = {
                            k: [try_decode(i) for i in attrs[k]]
                            for k in attrs
                        }
                    
                    # yield the value
                    yield dn, attrs
        
            # search for the requested server control
            pctrls = [
                c
                for c in serverctrls
                if c.controlType == SimplePagedResultsControl.controlType
            ]
            
            if not pctrls:
                # pctrls is empty, so the control was not found
                # most probably, the control it is not supported by the server
                # I'll break here and stop silently, but you may want to raise an exception
                #raise Exception('Warning:  Server ignores RFC 2696 control, so paging is not possible.')
                break
            else:
                # pctrls is not empty, so the control is there
                req_ctrl.cookie = pctrls[0].cookie
                
            if not req_ctrl.cookie:
                # cookie is empty, so there is no more pages to fetch
                # our job is done, just break the loop
                break
        else:
            # unexpected rtype
            # I'll break here and stop silently, but you may want to raise an exception
            break
    
    # free resources
    con.unbind_ext_s()

# helper function to try to decode results if possible
def try_decode(value):
    if isinstance(value, bytes):
        try:
            value = value.decode()
        except:
            print("I don't know what to do here...")
            # sometimes, we can't decode bytes to str
            # so we just ignore and return it as is
            pass
    
    return value


if __name__ == "__main__":
    # Globals
    ldapuser,ldappass = getpwtok()
    ldap_filterstr="&(objectClass=user)(!(objectClass=computer))"
    ldap_attrlist=["userPrincipalName", "givenName"]    # None to fetch all attributes
    ldap_timeout=-1                                     # wait indefinitely?
    ldap_pagesize=200

    response = query_activedirectory(
        uri=ldap_uri,
        bindDN=ldap_bindDN,
        bindPW=tmpauth[1],
        baseDN=ldap_baseDN,
        filterstr=f"(&(objectClass=user)(uid=*roy*))",
        attrlist=ldap_attrlist,
        timeout=-1,
        pagesize=ldap_pagesize,
        decodeBytes=True
    )

    count=0
    # fetch and display the results
    for dn, attrs in response:
        count += 1
        print(f"[{count:03d}] [DN] {dn}\t[ATTRS] {attrs}")


    print(f"Count is {count}")

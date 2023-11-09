#!/usr/bin/env python3

# THE PYTHON SDK IS IN PREVIEW. FOR NON-PRODUCTION USE ONLY

# async def ms365test1a():
#     graph_client = GraphServiceClient(credentials, scopes)
#     result = await graph_client.admin.service_announcement.health_overviews.by_service_health_id('serviceHealth-id').get()
# 
# def ms365test1b():
#     graph_client = GraphServiceClient(credentials, scopes)
#     result = graph_client.admin.service_announcement.health_overviews.by_service_health_id('serviceHealth-id').get()
# 
# ms365test1b()

from azure.identity.aio import ClientSecretCredential
from msgraph import GraphServiceClient
#import msgraph

graph_client = GraphServiceClient(credentials, scopes)

query_params = ServiceHealthItemRequestBuilder.ServiceHealthItemRequestBuilderGetQueryParameters(
		expand = ["issues"],
)

request_configuration = ServiceHealthItemRequestBuilder.ServiceHealthItemRequestBuilderGetRequestConfiguration(
query_parameters = query_params,
)

result = graph_client.admin.service_announcement.health_overviews.by_service_health_id('serviceHealth-id').get(request_configuration = request_configuration)


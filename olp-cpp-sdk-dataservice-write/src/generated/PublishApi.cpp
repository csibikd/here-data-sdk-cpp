/*
 * Copyright (C) 2019 HERE Europe B.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 * License-Filename: LICENSE
 */

#include "PublishApi.h"

#include <memory>
#include <sstream>

#include <olp/core/client/HttpResponse.h>
// clang-format off
// Ordering Required - Parser template specializations before JsonParser.h
#include <generated/parser/PublicationParser.h>
#include <generated/parser/PublishPartitionsParser.h>
#include <olp/core/generated/parser/JsonParser.h>
#include <generated/serializer/PublicationSerializer.h>
#include <generated/serializer/PublishPartitionsSerializer.h>
#include <generated/serializer/JsonSerializer.h>
// clang-format on

using namespace olp::client;

namespace {
const std::string kQueryParamBillingTag = "billingTag";
}  // namespace

namespace olp {
namespace dataservice {
namespace write {

CancellationToken PublishApi::InitPublication(
    const OlpClient& client, const model::Publication& publication,
    const boost::optional<std::string>& billing_tag,
    InitPublicationCallback callback) {
  std::multimap<std::string, std::string> header_params;
  std::multimap<std::string, std::string> query_params;
  std::multimap<std::string, std::string> form_params;

  header_params.insert(std::make_pair("Accept", "application/json"));

  if (billing_tag) {
    query_params.insert(
        std::make_pair(kQueryParamBillingTag, billing_tag.get()));
  }

  std::string init_publication_uri = "/publications";

  auto serialized_publication = serializer::serialize(publication);
  auto data = std::make_shared<std::vector<unsigned char>>(
      serialized_publication.begin(), serialized_publication.end());

  auto cancel_token = client.CallApi(
      init_publication_uri, "POST", query_params, header_params, form_params,
      data, "application/json", [callback](client::HttpResponse http_response) {
        if (http_response.status != 200) {
          callback(InitPublicationResponse{
              ApiError(http_response.status, http_response.response.str())});
          return;
        }

        callback(InitPublicationResponse(
            olp::parser::parse<model::Publication>(http_response.response)));
      });

  return cancel_token;
}

CancellationToken PublishApi::UploadPartitions(
    const OlpClient& client, const model::PublishPartitions& publish_partitions,
    const std::string& publication_id, const std::string& layer_id,
    const boost::optional<std::string>& billing_tag,
    UploadPartitionsCallback callback) {
  std::multimap<std::string, std::string> header_params;
  std::multimap<std::string, std::string> query_params;
  std::multimap<std::string, std::string> form_params;

  header_params.insert(std::make_pair("Accept", "application/json"));

  if (billing_tag) {
    query_params.insert(
        std::make_pair(kQueryParamBillingTag, billing_tag.get()));
  }

  std::string upload_partitions_uri =
      "/layers/" + layer_id + "/publications/" + publication_id + "/partitions";

  auto serialized_publish_partitions =
      serializer::serialize(publish_partitions);
  auto data = std::make_shared<std::vector<unsigned char>>(
      serialized_publish_partitions.begin(),
      serialized_publish_partitions.end());

  auto cancel_token = client.CallApi(
      upload_partitions_uri, "POST", query_params, header_params, form_params,
      data, "application/json", [callback](client::HttpResponse http_response) {
        if (http_response.status != 204) {
          callback(UploadPartitionsResponse{
              ApiError(http_response.status, http_response.response.str())});
          return;
        }

        callback(UploadPartitionsResponse(ApiNoResult()));
      });

  return cancel_token;
}

CancellationToken PublishApi::SubmitPublication(
    const OlpClient& client, const std::string& publication_id,
    const boost::optional<std::string>& billing_tag,
    SubmitPublicationCallback callback) {
  std::multimap<std::string, std::string> header_params;
  std::multimap<std::string, std::string> query_params;
  std::multimap<std::string, std::string> form_params;

  header_params.insert(std::make_pair("Accept", "application/json"));

  if (billing_tag) {
    query_params.insert(
        std::make_pair(kQueryParamBillingTag, billing_tag.get()));
  }

  std::string submit_publication_uri = "/publications/" + publication_id;

  auto cancel_token = client.CallApi(
      submit_publication_uri, "PUT", query_params, header_params, form_params,
      nullptr, "application/json",
      [callback](client::HttpResponse http_response) {
        if (http_response.status != 204) {
          callback(SubmitPublicationResponse{
              ApiError(http_response.status, http_response.response.str())});
          return;
        }

        callback(SubmitPublicationResponse(ApiNoResult()));
      });

  return cancel_token;
}

CancellationToken PublishApi::GetPublication(
    const OlpClient& client, const std::string& publication_id,
    const boost::optional<std::string>& billing_tag,
    GetPublicationCallback callback) {
  std::multimap<std::string, std::string> header_params;
  std::multimap<std::string, std::string> query_params;
  std::multimap<std::string, std::string> form_params;

  header_params.insert(std::make_pair("Accept", "application/json"));

  if (billing_tag) {
    query_params.insert(
        std::make_pair(kQueryParamBillingTag, billing_tag.get()));
  }

  std::string get_publication_uri = "/publications/" + publication_id;

  auto cancel_token = client.CallApi(
      get_publication_uri, "GET", query_params, header_params, form_params,
      nullptr, "application/json",
      [callback](client::HttpResponse http_response) {
        if (http_response.status != 200) {
          callback(GetPublicationResponse{
              ApiError(http_response.status, http_response.response.str())});
          return;
        }

        callback(GetPublicationResponse(
            olp::parser::parse<model::Publication>(http_response.response)));
      });

  return cancel_token;
}

CancellationToken PublishApi::CancelPublication(
    const client::OlpClient& client, const std::string& publication_id,
    const boost::optional<std::string>& billing_tag,
    CancelPublicationCallback callback) {
  std::multimap<std::string, std::string> header_params;
  std::multimap<std::string, std::string> query_params;
  std::multimap<std::string, std::string> form_params;

  header_params.insert(std::make_pair("Accept", "application/json"));

  if (billing_tag) {
    query_params.insert(
        std::make_pair(kQueryParamBillingTag, billing_tag.get()));
  }

  std::string submit_publication_uri = "/publications/" + publication_id;

  auto cancel_token = client.CallApi(
      submit_publication_uri, "DELETE", query_params, header_params,
      form_params, nullptr, "application/json",
      [callback](client::HttpResponse http_response) {
        if (http_response.status != 204) {
          callback(SubmitPublicationResponse{
              ApiError(http_response.status, http_response.response.str())});
          return;
        }

        callback(SubmitPublicationResponse(ApiNoResult()));
      });

  return cancel_token;
}

}  // namespace write
}  // namespace dataservice
}  // namespace olp
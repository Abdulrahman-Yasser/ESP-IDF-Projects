#pragma once 

#include "esp_err.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <cstring>

namespace NVS
{


class Nvs
{
    const char* const   _log_tag{nullptr};
    const char* const   _partition_name{nullptr};
    nvs_handle_t        handle{};
public:
    constexpr Nvs(const char* const partition_name = "nvs"):
        _log_tag{partition_name}, _partition_name{partition_name}{ }

    [[nodiscard]] esp_err_t init(void)
        { return _open(_partition_name, handle);}

    // with respect to a type
    template <typename T>
    [[nodiscard]] esp_err_t get(const char* const key, T& output)
        { return _get_buf(handle, key, output, 1) ;}

    template <typename T>
    [[nodiscard]] esp_err_t set(const char* const key, const T input)
        { return _set_buf(handle, key, input, 1) ;}

    template <typename T>
    [[nodiscard]] esp_err_t verify(const char* const key, const T input)
        { return _verify_buf(handle, key, input, 1) ;}


    // with respect to a pointer
    template <typename T>
    [[nodiscard]] esp_err_t get_buffer(const char* const key, T* output, const size_t len)
        { return _get_buf(handle, key, output, len) ;}

    template <typename T>
    [[nodiscard]] esp_err_t set_buffer(const char* const key, const T input, const size_t len)
        { return _set_buf(handle, key, input, len) ;}

    template <typename T>
    [[nodiscard]] esp_err_t verify_buffer(const char* const key, const T input, size_t& len)
        { return _verify_buf(handle, key, input, len) ;}

private:
    [[nodiscard]] static esp_err_t _open(const char* const partition_name, nvs_handle_t handle)
        {return nvs_open(partition_name,NVS_READWRITE, &handle) ;}


    template <typename T>
    [[nodiscard]] static esp_err_t _get(nvs_handle_t handle, const char* const key, T& output)
    {
        size_t n_bytes{sizeof(T)};

        if(nullptr == key || 0 == strlen(key))
            return ESP_ERR_INVALID_ARG;
        else
            return _get_buffer(handle, key, &output, n_bytes);
    }


    template <typename T>
    [[nodiscard]] static esp_err_t _set(nvs_handle_t handle, const char* const key, T& input)
    {
        size_t n_bytes{sizeof(T)};

        if(nullptr == key || 0 == strlen(key)){
            return ESP_ERR_INVALID_ARG;
        }
        else
        {
            esp_err_t state{_set_buffer(handle, key, &input, n_bytes)};

            if(ESP_OK == state) state |= nvs_commit(handle);

            if(ESP_OK == state) state |= _verify(handle, key, &input, n_bytes);

            return state;
        }
    }

    template <typename T>
    [[nodiscard]] static esp_err_t _verify(nvs_handle_t handle, const char* const key, T& input)
    {
        T val_in_nvs{};
        esp_err_t state{_get(handle, key, val_in_nvs)};
        if(ESP_OK == state )
        {
            if(input == val_in_nvs)
                return ESP_OK;
            else
                return ESP_FAIL;
        }
        return state;
    }


    // Note, length of num items of type T (not num of bytes)
    template <typename T>
    [[nodiscard]] static esp_err_t _get_buf(nvs_handle_t handle, const char* const key, T* output, size_t& len)
    {
        size_t n_bytes{sizeof(T) * len};
        esp_err_t status{ESP_FAIL};
        if(nullptr == key || 0 == strlen(key) || nullptr == output || len == 0)
        {
            return ESP_ERR_INVALID_ARG;
        }
        else
        {
            status = nvs_get_blob(handle, key, output, &n_bytes);
            len = n_bytes / sizeof(T);
        }
        return status;
    }

    template <typename T>
    [[nodiscard]] static esp_err_t _set_buf(nvs_handle_t handle, const char* const key,const T* input, const size_t len)
    {
        size_t n_bytes{sizeof(T) * len};
        esp_err_t status{ESP_FAIL};
        if(nullptr == key || 0 == strlen(key) || nullptr == input || len == 0)
        {
            return ESP_ERR_INVALID_ARG;
        }
        else
        {
            status = nvs_set_blob(handle, key, input, &n_bytes);
            if(ESP_OK == status)    status = nvs_commit(handle);
            if(ESP_OK == status)    status = _verify_buf(handle, key, input, len);
        }
        return status;
    }   


    /* compare our data with the ones in the nvs */
    template <typename T>
    [[nodiscard]] static esp_err_t _verify_buf(nvs_handle_t handle, const char* const key,const T* input, const size_t len)
    {
        esp_err_t status{ESP_FAIL};
        size_t n_items_in_nvs = len;
        T* buf_in_nvs = new T[len]{};

        if(buf_in_nvs)
        {

            status = _get_buf(handle, key, buf_in_nvs, n_items_in_nvs);
            if(ESP_OK == status)
            {
                if(len == n_items_in_nvs)
                {
                    if(0 != memcmp(input, buf_in_nvs, len * sizeof(T)))
                    {
                        // data is match
                        status = ESP_FAIL;
                    }
                }
                else
                {
                    return ESP_ERR_NVS_INVALID_LENGTH;
                }
            }
            delete[] buf_in_nvs;
        }
        else
        {
            status = ESP_ERR_NO_MEM;
        }
        return status;
    }

};   // class Nvs

}   // namespace NVS
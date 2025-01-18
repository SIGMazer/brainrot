#include "mem.h"

/**
 * @brief Allocates memory safely with additional security features
 *
 * This function provides a safer alternative to malloc with the following features:
 * - Checks for integer overflow
 * - Aligns memory properly
 * - Zero-initializes allocated memory
 * - Stores allocation size for safe_free
 * - Handles edge cases (zero size, overflow)
 *
 * @param size The number of bytes to allocate
 * @return void* Pointer to the allocated memory, or NULL if:
 *         - size is 0
 *         - size exceeds MAX_ALLOC_SIZE
 *         - memory allocation fails
 *         - alignment calculation overflows
 */
void *safe_malloc(size_t size)
{
    if (size == 0)
    {
        return NULL;
    }

    if (size > MAX_ALLOC_SIZE)
    {
        return handle_malloc_error(size);
    }

    size_t aligned_size = align_size(size);
    if (aligned_size == 0 || aligned_size > MAX_ALLOC_SIZE)
    {
        return handle_malloc_error(size);
    }

    void *ptr = malloc(aligned_size + sizeof(size_t));
    if (ptr == NULL)
    {
        return handle_malloc_error(size);
    }

    size_t *size_ptr = (size_t *)ptr;
    *size_ptr = aligned_size;
    ptr = size_ptr + 1;

    memset(ptr, 0, aligned_size);
    return ptr;
}

/**
 * @brief Aligns the requested size to the platform's alignment requirements
 *
 * Rounds up the size to the nearest multiple of ALIGNMENT (typically sizeof(void*))
 * to ensure proper memory alignment for all types.
 *
 * @param size The size to align
 * @return size_t The aligned size, or 0 if alignment calculation would overflow
 */
static size_t align_size(size_t size)
{
    return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

/**
 * @brief Handles memory allocation errors with detailed reporting
 *
 * Logs the error to stderr with the failed allocation size and system error message,
 * sets errno to ENOMEM, and returns NULL.
 *
 * @param size The size of the failed allocation attempt
 * @return void* Always returns NULL
 */
static void *handle_malloc_error(size_t size)
{
    fprintf(stderr, "Memory allocation failed - Size: %zu, Error: %s\n",
            size, strerror(errno));
    errno = ENOMEM;
    return NULL;
}

/**
 * @brief Safely allocates an array of elements
 *
 * Provides array allocation with overflow checking for the total size calculation.
 * Ensures that nmemb * size doesn't overflow before attempting allocation.
 *
 * @param nmemb Number of elements to allocate
 * @param size Size of each element
 * @return void* Pointer to the allocated array, or NULL on any error condition
 */
void *safe_malloc_array(size_t nmemb, size_t size)
{
    if (nmemb > 0 && size > MAX_ALLOC_SIZE / nmemb)
    {
        return handle_malloc_error(nmemb * size);
    }
    return safe_malloc(nmemb * size);
}

/**
 * @brief Safely frees memory and nullifies the pointer
 *
 * Enhanced version of free that:
 * - Validates the pointer-to-pointer
 * - Retrieves the original allocation size
 * - Securely wipes memory before freeing
 * - Nullifies the pointer after freeing
 * - Handles NULL pointers safely
 *
 * @param ptr Pointer to the pointer to free. Must not be NULL.
 *            The pointed-to pointer will be set to NULL after freeing.
 */
void safe_free(void **ptr)
{
    if (ptr == NULL)
    {
        fprintf(stderr, "Warning: Attempt to free NULL pointer reference\n");
        return;
    }

    if (*ptr != NULL)
    {
        size_t *size_ptr = (size_t *)(*ptr) - 1;
        size_t size = *size_ptr;

        memset(*ptr, 0, size);
        free((void *)size_ptr);
        *ptr = NULL;
    }
}

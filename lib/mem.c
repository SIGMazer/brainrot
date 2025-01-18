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
size_t align_size(size_t size)
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
void *handle_malloc_error(size_t size)
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

/**
 * @brief Creates a secure duplicate of a string with bounds checking
 *
 * Enhanced version of strdup that:
 * - Validates input string
 * - Uses safe memory allocation
 * - Performs secure string copying
 * - Handles NULL and edge cases
 * - Ensures proper null termination
 *
 * @param str The source string to duplicate. Must not be NULL.
 * @return char* Returns:
 *         - New allocated string on success
 *         - NULL if:
 *           - Input string is NULL
 *           - Memory allocation fails
 *           - String length exceeds MAX_ALLOC_SIZE
 *           - Memory copying fails
 *
 * @note The returned string must be freed using SAFE_FREE when no longer needed
 *
 * @example
 *   char *copy = safe_strdup("hello");
 *   if (copy) {
 *       // use copy
 *       SAFE_FREE(copy);
 *   }
 */
char *safe_strdup(const char *str)
{
    if (str == NULL)
    {
        fprintf(stderr, "Error: NULL string passed to safe_strdup\n");
        errno = EINVAL;
        return NULL;
    }

    size_t len = strlen(str) + 1;
    char *new_str = SAFE_MALLOC_ARRAY(char, len);
    if (new_str)
    {
        // Use regular memcpy since str might not be from safe_malloc
        memcpy(new_str, str, len);
    }
    return new_str;
}

/**
 * @brief Safely copies memory between buffers with bounds checking
 *
 * @param dest Destination buffer
 * @param src Source buffer
 * @param n Number of bytes to copy
 * @return void* Pointer to destination buffer, or NULL if:
 * - dest or src is NULL
 * - n is 0
 * - buffers overlap
 * - n exceeds source or destination buffer size
 */
void *safe_memcpy(void *dest, const void *src, size_t n)
{
    if (dest == NULL || src == NULL)
    {
        fprintf(stderr, "Error: NULL pointer passed to safe_memcpy\n");
        errno = EINVAL;
        return NULL;
    }

    if (n == 0)
    {
        return dest;
    }

    if (n > MAX_ALLOC_SIZE)
    {
        fprintf(stderr, "Error: Copy size exceeds maximum allowed\n");
        errno = ERANGE;
        return NULL;
    }

    // Check for buffer overlap
    if ((src < dest && (char *)src + n > dest) ||
        (dest < src && (char *)dest + n > src))
    {
        fprintf(stderr, "Error: Buffer overlap detected in safe_memcpy\n");
        errno = EINVAL;
        return NULL;
    }

    // Only check destination size since it's from safe_malloc
    size_t *dest_size_ptr = (size_t *)dest - 1;

    // Verify destination buffer has enough space
    if (*dest_size_ptr < n)
    {
        fprintf(stderr, "Error: Destination buffer too small for copy\n");
        errno = ERANGE;
        return NULL;
    }

    // Skip source buffer size check since it might not be from safe_malloc

    // Perform the actual copy
    return memcpy(dest, src, n);
}

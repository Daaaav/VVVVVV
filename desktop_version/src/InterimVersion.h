#ifndef INTERIMVERSION_H
#define INTERIMVERSION_H

#ifdef INTERIM_VERSION_EXISTS

#ifdef __cplusplus
extern "C"
{
#endif

extern const char* INTERIM_COMMIT;
extern const int LEN_INTERIM_COMMIT;

extern const char* COMMIT_DATE;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* INTERIM_VERSION_EXISTS */

#endif /* INTERIMVERSION_H */

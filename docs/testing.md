# ManiMart Testing Strategy

## 1. Test Layers

1. **Unit Tests (GoogleTest & GoogleMock)**:
   - `AuthServiceTest`: Registration, duplicate checking, password hashing, and login validation.
   - `ProductServiceTest`: Product creation, ownership validation, stock bounds, and buyer restriction.
   - `CartServiceTest`: Add, update, remove items, stock overflow prevention, and subtotal calculation.
   - `OrderServiceTest`: Atomic checkout workflow, empty cart rejection, mock payment processing, and stock updates.
   - `ReviewServiceTest`: Verified buyer eligibility gate, rating bounds (1-5), and duplicate review prevention.

2. **Integration Tests**:
   - `RepositoryIntegrationTests`: Real SQL schema creation, parameterized CRUD operations, and index validation against in-memory databases.

3. **Sanitizers**:
   - AddressSanitizer (`-fsanitize=address`)
   - UndefinedBehaviorSanitizer (`-fsanitize=undefined`)
   - ThreadSanitizer (`-fsanitize=thread`)

## 2. Running Tests
```bash
./scripts/test.sh
```
or
```bash
cd build && ctest --output-on-failure
```

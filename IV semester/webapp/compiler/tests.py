from django.test import TestCase, Client
from .views import *
from django.contrib.auth import authenticate
from datetime import timedelta
from django.utils import timezone


# ================ MODELS TESTS ================

class UserModelTest(TestCase):

    @classmethod
    def setUpTestData(cls):
        User.objects.create(username='test_user', email='test_login', password='test_password')

    def test_username(self):
        user = User.objects.get(id=1)
        self.assertIsNotNone(user.username)

    def test_login(self):
        user = User.objects.get(id=1)
        self.assertIsNotNone(user.email)

    def test_password(self):
        user = User.objects.get(id=1)
        self.assertIsNotNone(user.password)

    def test_user_str(self):
        user = User.objects.get(id=1)
        expected_str = 'test_user'
        self.assertEqual(str(user), expected_str)


class CatalogModelTest(TestCase):

    @classmethod
    def setUpTestData(cls):
        Catalog.objects.create(name='Test Catalog', description='This is a test catalog', availability=False)

    def test_catalog_creation_date(self):
        catalog = Catalog.objects.get(id=1)
        self.assertAlmostEqual(catalog.creation_date, timezone.now(), delta=timezone.timedelta(seconds=1))

    def test_catalog_availability_change_date(self):
        catalog = Catalog.objects.get(id=1)
        self.assertAlmostEqual(catalog.availability_change_date, timezone.now(), delta=timezone.timedelta(seconds=1))

    def test_catalog_last_changed(self):
        catalog = Catalog.objects.get(id=1)
        self.assertAlmostEqual(catalog.last_changed, timezone.now(), delta=timezone.timedelta(seconds=1))

    def test_catalog_str(self):
        catalog = Catalog.objects.get(id=1)
        expected_str = 'Test Catalog'
        self.assertEqual(str(catalog), expected_str)


class FileModelTest(TestCase):

    @classmethod
    def setUpTestData(cls):
        catalog = Catalog.objects.create(name='Test Catalog', description='This is a test catalog', availability=False)
        File.objects.create(catalog=catalog, name='Test File', description='This is a test file',
                            source_code='print("Hello, world!")')

    def test_file_creation_date(self):
        file = File.objects.get(id=1)
        self.assertAlmostEqual(file.creation_date, timezone.now(), delta=timezone.timedelta(seconds=1))

    def test_file_availability_change_date(self):
        file = File.objects.get(id=1)
        self.assertAlmostEqual(file.availability_change_date, timezone.now(), delta=timezone.timedelta(seconds=1))

    def test_file_last_changed(self):
        file = File.objects.get(id=1)
        self.assertAlmostEqual(file.last_changed, timezone.now(), delta=timezone.timedelta(seconds=1))

    def test_file_str(self):
        file = File.objects.get(id=1)
        expected_str = 'Test File'
        self.assertEqual(str(file), expected_str)

    def test_file_source_code(self):
        file = File.objects.get(id=1)
        expected_source_code = 'print("Hello, world!")'
        self.assertEqual(file.source_code, expected_source_code)

    def test_file_catalog(self):
        file = File.objects.get(id=1)
        self.assertEqual(file.catalog.name, 'Test Catalog')


class SectionTypeModelTest(TestCase):

    @classmethod
    def setUpTestData(cls):
        SectionType.objects.create(category='PROCEDURA')

    def test_category_length(self):
        section_type = SectionType.objects.get(id=1)
        section_type_len = len(section_type.category)
        self.assertEqual(section_type_len, 9)

    def test_section_type_str(self):
        section_type = SectionType.objects.get(id=1)
        expected_str = 'PROCEDURA'
        self.assertEqual(str(section_type), expected_str)


class SectionStatusModelTest(TestCase):

    @classmethod
    def setUpTestData(cls):
        SectionStatus.objects.create(status='KOMPILUJE_SIE_Z_OSTRZEZENIAMI')

    def test_status_length(self):
        section_status = SectionStatus.objects.get(id=1)
        section_status_len = len(section_status.status)
        self.assertEqual(section_status_len, 29)

    def test_section_status_str(self):
        section_status = SectionStatus.objects.get(id=1)
        expected_str = 'KOMPILUJE_SIE_Z_OSTRZEZENIAMI'
        self.assertEqual(str(section_status), expected_str)


class StatusDataModelTest(TestCase):

    @classmethod
    def setUpTestData(cls):
        StatusData.objects.create(status='Error', line_of_error=5)

    def test_status_length(self):
        status_data = StatusData.objects.get(id=1)
        status_data_len = len(status_data.status)
        self.assertEqual(status_data_len, 5)

    def test_line_of_error_label(self):
        status_data = StatusData.objects.get(id=1)
        field_label = status_data._meta.get_field('line_of_error').verbose_name
        self.assertEqual(field_label, 'line of error')

    def test_line_of_error_positive_integer_field(self):
        status_data = StatusData.objects.get(id=1)
        line_of_error_field = status_data._meta.get_field('line_of_error')
        self.assertIsInstance(line_of_error_field, models.PositiveIntegerField)
        self.assertIn(MinValueValidator(0), line_of_error_field.validators)

    def test_status_data_str(self):
        status_data = StatusData.objects.get(id=1)
        expected_str = 'Error:5'
        self.assertEqual(str(status_data), expected_str)


class SectionModelTestCase(TestCase):

    @classmethod
    def setUp(cls):
        catalog = Catalog.objects.create(name='Test Catalog', description='This is a test catalog', availability=False)
        file = File.objects.create(catalog=catalog, name='Test File', description='This is a test file',
                                   source_code='print("Hello, world!")')
        type_of_section = SectionType.objects.create(category='Procedura')
        section_status = SectionStatus.objects.create(status='Nie kompiluje się')
        status_data = StatusData.objects.create(status='Error', line_of_error=10)
        Section.objects.create(
            file=file,
            name='Test Section',
            section_start=0,
            section_end=100,
            type_of_section=type_of_section,
            section_status=section_status,
            status_data=status_data,
        )

    def test_section_str_method(self):
        section = Section.objects.get(id=1)
        self.assertEqual(str(section), 'Test Section')

    def test_section_test_required(self):
        section = Section.objects.get(id=1)
        self.assertIsNotNone(section.file)
        self.assertIsNotNone(section.type_of_section)
        self.assertIsNotNone(section.section_status)
        self.assertIsNotNone(section.status_data)
        self.assertIsNotNone(section.section_start)
        self.assertIsNotNone(section.section_end)

    def test_section_description_is_optional(self):
        section = Section.objects.get(id=1)
        self.assertIsNone(section.description)
        self.assertIsNone(section.contents)

    def test_section_creation_date_is_now(self):
        section = Section.objects.get(id=1)
        self.assertAlmostEqual(section.creation_date, timezone.now(), delta=timezone.timedelta(seconds=1))

    def test_section_section_start_is_positive_integer_field(self):
        section = Section.objects.get(id=1)
        section_start_line = section._meta.get_field('section_start')
        self.assertIsInstance(section_start_line, models.PositiveIntegerField)
        self.assertIn(MinValueValidator(0), section_start_line.validators)

    def test_section_section_end_is_positive_integer_field(self):
        section = Section.objects.get(id=1)
        section_end_line = section._meta.get_field('section_end')
        self.assertIsInstance(section_end_line, models.PositiveIntegerField)
        self.assertIn(MinValueValidator(0), section_end_line.validators)


# ================ VIEWS TESTS ================

class IndexViewTestCase(TestCase):

    def setUp(self):
        self.client = Client()
        self.index_url = reverse('index')
        self.user1 = User.objects.create_user(
            username='user1',
            email='user1@example.com',
            password='user1password'
        )
        self.user2 = User.objects.create_user(
            username='user2',
            email='user2@example.com',
            password='user2password'
        )
        self.catalog1 = Catalog.objects.create(
            owner=self.user1,
            name='catalog1',
            availability=True
        )
        self.catalog2 = Catalog.objects.create(
            owner=self.user2,
            name='catalog2',
            availability=True
        )
        self.file1 = File.objects.create(
            owner=self.user1,
            name='file1',
            availability=True
        )
        self.file2 = File.objects.create(
            owner=self.user2,
            name='file2',
            availability=True
        )

    def test_login_required(self):
        response = self.client.get(self.index_url)
        self.assertRedirects(response, '/compiler/loginview/?next=/')

    def test_user_catalogs_files_in_context(self):
        self.client.login(username='user1', password='user1password')
        response = self.client.get(self.index_url)
        catalogs = response.context['catalogs']
        files = response.context['files']
        self.assertIn(self.catalog1, catalogs)
        self.assertIn(self.file1, files)

    def test_only_user_catalogs_files_in_context(self):
        self.client.login(username='user1', password='user1password')
        response = self.client.get(self.index_url)
        catalogs = response.context['catalogs']
        files = response.context['files']
        self.assertNotIn(self.catalog2, catalogs)
        self.assertNotIn(self.file2, files)

    def test_only_available_catalogs_files_in_context(self):
        self.client.login(username='user1', password='user1password')
        self.catalog1.availability = False
        self.catalog1.save()
        response = self.client.get(self.index_url)
        catalogs = response.context['catalogs']
        self.assertNotIn(self.catalog1, catalogs)


class RegisterViewTestCase(TestCase):

    def setUp(self):
        self.client = Client()
        self.register_url = reverse('register')
        self.valid_data = {
            'username': 'testuser',
            'login': 'testuserexample.com',
            'password': 'testuserpassword'
        }

    def test_register_get(self):
        response = self.client.get(self.register_url)
        self.assertEqual(response.status_code, 200)
        self.assertTemplateUsed(response, 'compiler/register.html')

    def test_register_post(self):
        response = self.client.post(self.register_url, data=self.valid_data)
        self.assertEqual(response.status_code, 302)
        user = User.objects.get(username=self.valid_data['username'])
        self.assertIsNotNone(user)
        self.assertEqual(user.email, self.valid_data['login'])


class LoginViewTestCase(TestCase):

    def setUp(self):
        self.client = Client()
        self.login_url = reverse('loginview')
        self.user_data = {
            'username': 'testuser',
            'password': 'testuserpassword'
        }
        self.user = User.objects.create_user(**self.user_data)

    def test_login_get(self):
        response = self.client.get(self.login_url)
        self.assertEqual(response.status_code, 200)
        self.assertTemplateUsed(response, 'compiler/login.html')

    def test_login_post_valid_credentials(self):
        response = self.client.post(self.login_url, data=self.user_data)
        self.assertRedirects(response, reverse('index'))
        user = authenticate(username=self.user_data['username'], password=self.user_data['password'])
        self.assertIsNotNone(user)
        self.assertEqual(user, self.user)

    def test_login_authenticate(self):
        user = authenticate(username=self.user_data['username'], password=self.user_data['password'])
        self.assertIsNotNone(user)
        self.assertEqual(user, self.user)

    def test_login_successful(self):
        response = self.client.post(self.login_url, data=self.user_data)
        self.assertEqual(response.status_code, 302)
        self.assertEqual(response.url, reverse('index'))


class LogoutViewTestCase(TestCase):

    def setUp(self):
        self.client = Client()
        self.logout_url = reverse('logoutview')
        self.user_data = {
            'username': 'testuser',
            'password': 'testuserpassword'
        }
        self.user = User.objects.create_user(**self.user_data)

    def test_logout(self):
        self.client.login(username=self.user_data['username'], password=self.user_data['password'])
        response = self.client.get(self.logout_url)
        self.assertEqual(response.status_code, 200)


class UsernameValidationViewTestCase(TestCase):

    def setUp(self):
        self.client = Client()
        self.url = reverse('usernamevalidation')

    def test_valid_username(self):
        data = {
            'username': 'testuser123'
        }
        response = self.client.post(self.url, data=json.dumps(data), content_type='application/json')
        self.assertEqual(response.status_code, 200)
        self.assertDictEqual(response.json(), {'username_valid': True})

    def test_invalid_username(self):
        data = {
            'username': 'testuser#'
        }
        response = self.client.post(self.url, data=json.dumps(data), content_type='application/json')
        self.assertEqual(response.status_code, 400)
        self.assertDictEqual(response.json(), {'username_error': 'Username must contain only letters and digits'})

    def test_existing_username(self):
        user = User.objects.create_user(username='existinguser', password='existinguserpassword')
        data = {
            'username': 'existinguser'
        }
        response = self.client.post(self.url, data=json.dumps(data), content_type='application/json')
        self.assertEqual(response.status_code, 409)
        self.assertDictEqual(response.json(), {'username_error': 'Username already exists'})


class LoginValidationViewTestCase(TestCase):

    def setUp(self):
        self.client = Client()
        self.url = reverse('loginvalidation')

    def test_valid_login(self):
        data = {
            'login': 'testuser123examplecom'
        }
        response = self.client.post(self.url, data=json.dumps(data), content_type='application/json')
        self.assertEqual(response.status_code, 200)
        self.assertDictEqual(response.json(), {'login_valid': True})

    def test_invalid_login(self):
        data = {
            'login': 'testuser#'
        }
        response = self.client.post(self.url, data=json.dumps(data), content_type='application/json')
        self.assertEqual(response.status_code, 400)
        self.assertDictEqual(response.json(), {'login_error': 'Login must contain only letters and digits'})

    def test_existing_login(self):
        user = User.objects.create_user(
            username='existinguser', email="existinguserexamplecom", password='existinguserpassword'
        )
        data = {
            'login': 'existinguserexamplecom'
        }
        response = self.client.post(self.url, data=json.dumps(data), content_type='application/json')
        self.assertEqual(response.status_code, 409)
        self.assertDictEqual(response.json(), {'login_error': 'Login already exists'})


class AddDirectoryTestCase(TestCase):

    def setUp(self):
        self.client = Client()
        self.user = User.objects.create_user(username='testuser', password='testpass')
        self.catalog = Catalog.objects.create(name='test catalog', owner=self.user, availability=True)

    def test_view_url_exists_at_desired_location(self):
        self.client.login(username='testuser', password='testpass')
        response = self.client.get('/compiler/adddirectory/')
        self.assertEqual(response.status_code, 200)

    def test_view_url_accessible_by_name(self):
        self.client.login(username='testuser', password='testpass')
        response = self.client.get(reverse('adddirectory'))
        self.assertEqual(response.status_code, 200)

    def test_view_uses_correct_template(self):
        self.client.login(username='testuser', password='testpass')
        response = self.client.get(reverse('adddirectory'))
        self.assertEqual(response.status_code, 200)
        self.assertTemplateUsed(response, 'compiler/adddirectory.html')

    def test_view_adds_new_catalog(self):
        self.client.login(username='testuser', password='testpass')
        response = self.client.post(reverse('adddirectory'), {'name': 'new catalog', 'description': 'test description'})
        self.assertEqual(response.status_code, 302)
        self.assertTrue(Catalog.objects.filter(name='new catalog').exists())

    def test_view_adds_new_catalog_with_parent(self):
        self.client.login(username='testuser', password='testpass')
        response = self.client.post(
            reverse('adddirectory'),
            {'name': 'new catalog', 'description': 'test description', 'parent_catalog': self.catalog.id})
        self.assertEqual(response.status_code, 302)
        self.assertTrue(Catalog.objects.filter(name='new catalog', parent_catalog=self.catalog).exists())

    def test_view_returns_error_if_parent_catalog_does_not_exist(self):
        self.client.login(username='testuser', password='testpass')
        response = self.client.post(reverse(
            'adddirectory'), {'name': 'new catalog', 'description': 'test description', 'parent_catalog': 9999})
        self.assertEqual(response.status_code, 404)

    def test_view_requires_authentication(self):
        response = self.client.get(reverse('adddirectory'))
        self.assertEqual(response.status_code, 302)
        self.assertRedirects(response, '/compiler/loginview/?next=/compiler/adddirectory/')

    def test_view_displays_catalogs(self):
        self.client.login(username='testuser', password='testpass')
        response = self.client.get(reverse('adddirectory'))
        self.assertEqual(response.status_code, 200)
        self.assertContains(response, 'test catalog')


class AddFileTestCase(TestCase):

    def setUp(self):
        self.client = Client()
        self.user = User.objects.create_user(username='testuser', password='testpassword')
        self.catalog = Catalog.objects.create(name='Test Catalog', owner=self.user)
        self.valid_payload = {
            'name': 'Test File',
            'source_code': 'print("Hello, World!")',
            'description': 'Test description',
            'catalog': self.catalog.id,
        }
        self.invalid_payload = {
            'name': '',
            'source_code': '',
            'description': '',
        }

    def test_add_file_get(self):
        self.client.login(username='testuser', password='testpassword')
        response = self.client.get(reverse('addfile'))
        self.assertEqual(response.status_code, 200)
        self.assertTemplateUsed(response, 'compiler/addfile.html')

    def test_add_file_post_valid_payload(self):
        self.client.login(username='testuser', password='testpassword')
        response = self.client.post(reverse('addfile'), data=self.valid_payload)
        self.assertEqual(response.status_code, 302)
        self.assertRedirects(response, reverse('index'))
        self.assertEqual(File.objects.count(), 1)
        file = File.objects.first()
        self.assertEqual(file.name, 'Test File')
        self.assertEqual(file.source_code, 'print("Hello, World!")')
        self.assertEqual(file.description, 'Test description')
        self.assertEqual(file.catalog, self.catalog)

    def test_add_file_post_invalid_payload(self):
        self.client.login(username='testuser', password='testpassword')
        response = self.client.post(reverse('addfile'), data=self.invalid_payload)
        self.assertEqual(response.status_code, 302)


class DeleteDirectoryViewTestCase(TestCase):

    def setUp(self):
        self.user = User.objects.create_user('testuser', 'testexamplecom', 'testpassword')
        self.catalog1 = Catalog.objects.create(name='Test Catalog 1', owner=self.user)
        self.catalog2 = Catalog.objects.create(name='Test Catalog 2', owner=self.user)
        self.catalog3 = Catalog.objects.create(name='Test Catalog 3', owner=self.user, parent_catalog=self.catalog1)
        self.file1 = File.objects.create(name='Test File 1', owner=self.user, catalog=self.catalog1)
        self.file2 = File.objects.create(name='Test File 2', owner=self.user, catalog=self.catalog2)
        self.file3 = File.objects.create(name='Test File 3', owner=self.user, catalog=self.catalog3)

    def test_delete_directory_get(self):
        self.client.login(username='testuser', password='testpassword')
        response = self.client.get(reverse('deletedirectory'))
        self.assertEqual(response.status_code, 200)
        self.assertTemplateUsed(response, 'compiler/deletedirectory.html')

    def test_delete_directory_post(self):
        self.client.login(username='testuser', password='testpassword')
        response = self.client.post(reverse('deletedirectory'), {'catalog': self.catalog1.id})
        self.assertEqual(response.status_code, 302)
        self.assertRedirects(response, reverse('index'))
        self.catalog1.refresh_from_db()
        self.assertFalse(self.catalog1.availability)
        self.assertEqual(self.catalog1.availability_change_date.date(), timezone.now().date())
        self.assertFalse(self.catalog1.last_changed < timezone.now() - timedelta(seconds=1))
        self.assertEqual(self.file1.availability_change_date.date(), timezone.now().date())
        self.assertFalse(self.file1.last_changed < timezone.now() - timedelta(seconds=1))
        self.catalog3.refresh_from_db()
        self.assertFalse(self.catalog3.availability)
        self.assertEqual(self.catalog3.availability_change_date.date(), timezone.now().date())
        self.assertFalse(self.catalog3.last_changed < timezone.now() - timedelta(seconds=1))
        self.assertEqual(self.file3.availability_change_date.date(), timezone.now().date())
        self.assertFalse(self.file3.last_changed < timezone.now() - timedelta(seconds=1))
        self.catalog2.refresh_from_db()
        self.assertTrue(self.catalog2.availability)
        self.assertFalse(self.catalog2.last_changed < timezone.now() - timedelta(seconds=1))
        self.assertEqual(self.file2.availability_change_date.date(), timezone.now().date())
        self.assertFalse(self.file2.last_changed < timezone.now() - timedelta(seconds=1))
        self.assertTrue(self.file2.availability)

    def test_delete_directory_post_no_catalog(self):
        self.client.login(username='testuser', password='testpassword')
        response = self.client.post(reverse('deletedirectory'))
        self.assertEqual(response.status_code, 302)
        self.assertRedirects(response, reverse('index'))

    def test_delete_directory_post_invalid_catalog(self):
        self.client.login(username='testuser', password='testpassword')
        response = self.client.post(reverse('deletedirectory'), {'catalog': 999})
        self.assertEqual(response.status_code, 404)


class DeleteFileViewTest(TestCase):

    def setUp(self):
        self.client = Client()
        self.user = User.objects.create_user(username='testuser', password='testpassword')
        self.catalog = Catalog.objects.create(name='Test Catalog', owner=self.user)
        self.file = File.objects.create(name='Test File', owner=self.user, catalog=self.catalog)

    def test_delete_file_get(self):
        self.client.login(username='testuser', password='testpassword')
        response = self.client.get(reverse('deletefile'))
        self.assertEqual(response.status_code, 200)
        self.assertTemplateUsed(response, 'compiler/deletefile.html')
        self.assertContains(response, 'Test File')

    def test_delete_file_post(self):
        self.client.login(username='testuser', password='testpassword')
        response = self.client.post(reverse('deletefile'), {'file': self.file.id})
        self.assertRedirects(response, reverse('index'))
        self.assertEqual(File.objects.filter(availability=True).count(), 0)

    def test_delete_file_post_nonexistent_file(self):
        self.client.login(username='testuser', password='testpassword')
        response = self.client.post(reverse('deletefile'), {'file': 999})
        self.assertEqual(response.status_code, 404)
        self.assertEqual(File.objects.filter(availability=True).count(), 1)

    def test_delete_file_post_no_file_id(self):
        self.client.login(username='testuser', password='testpassword')
        response = self.client.post(reverse('deletefile'))
        self.assertRedirects(response, reverse('index'))
        self.assertEqual(File.objects.filter(availability=True).count(), 1)


class GetFileTestCase(TestCase):

    def setUp(self):
        self.client = Client()

        self.user = User.objects.create_user(
            username='testuser', password='testpassword')
        self.catalog = Catalog.objects.create(name='Test Catalog', owner=self.user)
        self.file = File.objects.create(name='Test File', source_code='print("Hello World!")',
                                        owner=self.user, catalog=self.catalog)

        self.url = reverse('getfile', args=[self.file.id])

    def test_get_file_successfully(self):
        self.client.login(username='testuser', password='testpassword')
        response = self.client.get(self.url)
        self.assertEqual(response.status_code, 200)
        self.assertEqual(response.json(), {'data': 'print("Hello World!")'})

    def test_get_file_not_found(self):
        self.client.login(username='testuser', password='testpassword')
        response = self.client.get(reverse('getfile', args=[100]))
        self.assertEqual(response.status_code, 409)
        self.assertEqual(response.json(), {'result': 'File does not exist'})

    def test_get_file_unauthenticated(self):
        response = self.client.get(self.url)
        self.assertEqual(response.status_code, 302)
        self.assertRedirects(response, '/compiler/loginview/?next=' + self.url)


class SaveFileViewTestCase(TestCase):

    def setUp(self):
        self.client = Client()
        self.user = User.objects.create_user(username='testuser', password='12345')
        self.file = File.objects.create(name='test_file', source_code='print("Hello World")', owner=self.user)

    def test_savefile_GET_request(self):
        url = reverse('savefile', args=[self.file.id])
        response = self.client.get(url)
        self.assertEqual(response.status_code, 302)

    def test_savefile_POST_request_non_changed_file(self):
        url = reverse('savefile', args=[self.file.id])
        self.file.refresh_from_db()
        self.assertEqual(self.file.source_code, 'print("Hello World")')


class CompileFileViewTestCase(TestCase):

    def setUp(self):
        self.client = Client()
        self.user = User.objects.create_user(username='testuser', password='testpass')
        self.file = File.objects.create(
            name='testfile',
            source_code='#include <stdio.h> int main() {printf("siema\n");return 0;}',
            owner=self.user,
        )

    def test_compilefile_view_with_valid_input(self):
        url = reverse('compilefile', args=[self.file.id, 'std', 'proc', 'proc_flag', 'flags', 0])
        response = self.client.get(url)
        self.assertEqual(response.status_code, 302)
        self.assertFalse(os.path.isfile("temp.asm"))


# ================ FORMS TESTS ================

class SaveFileFormTest(TestCase):

    def setUp(self):
        self.user = User.objects.create_user(
            username='testuser', password='testpass123')

        self.file = File.objects.create(
            name='testfile.c',
            source_code='int main() { return 0; }',
            owner=self.user,
            catalog=None,
            availability=True,
        )

    def test_save_file_form_valid(self):
        self.client.login(username='testuser', password='testpass123')
        form_data = {
            'file_content': 'int main() { printf("Hello, world!"); }',
        }
        response = self.client.post(
            reverse('savefile', args=[self.file.id]),
            data=form_data,
        )
        self.assertEqual(response.status_code, 200)
        self.assertEqual(response.json()['success'], True)


class AddCatalogFormTest(TestCase):

    def setUp(self):
        self.user = User.objects.create_user(username='testuser', password='testpass')
        self.catalogs = [
            Catalog.objects.create(name='Katalog1', owner=self.user),
            Catalog.objects.create(name='Katalog2', owner=self.user)
        ]

    def test_add_catalog_form_submit(self):
        form_data = {
            'name': 'TestCatalog',
            'description': 'Test description',
            'parent_catalog': self.catalogs[0].id
        }
        self.client.login(username='testuser', password='testpass')
        response = self.client.post(reverse('adddirectory'), form_data)
        self.assertEqual(response.status_code, 302)
        self.assertRedirects(response, reverse('index'))

        catalog = Catalog.objects.get(name='TestCatalog')
        self.assertEqual(catalog.owner, self.user)
        self.assertEqual(catalog.description, 'Test description')
        self.assertEqual(catalog.parent_catalog, self.catalogs[0])


class AddFileFormTest(TestCase):

    def setUp(self):
        self.client = Client()
        self.user = User.objects.create_user(username='testuser', password='12345')
        self.catalog = Catalog.objects.create(name='Test Catalog', owner=self.user)

    def test_add_file_form_valid(self):
        self.client.login(username='testuser', password='12345')
        form_data = {'name': 'Test File', 'source_code': 'print("Hello, world!")', 'description': 'Test description',
                     'catalog': self.catalog.id}
        response = self.client.post(reverse('addfile'), data=form_data)
        self.assertEqual(response.status_code, 302)
        self.assertEqual(File.objects.count(), 1)
        self.assertEqual(File.objects.first().name, 'Test File')
        self.assertEqual(File.objects.first().source_code, 'print("Hello, world!")')
        self.assertEqual(File.objects.first().description, 'Test description')
        self.assertEqual(File.objects.first().catalog, self.catalog)
        self.assertEqual(File.objects.first().owner, self.user)


class DeleteCatalogFormTest(TestCase):

    def setUp(self):
        self.catalog1 = Catalog.objects.create(name='Test Catalog 1')
        self.catalog2 = Catalog.objects.create(name='Test Catalog 2')
        self.url = reverse('deletedirectory')

    def test_delete_catalog_form_deletes_catalog(self):
        data = {'catalog': self.catalog1.id}
        response = self.client.post(self.url, data)
        self.assertEqual(response.status_code, 302)
        self.assertFalse(Catalog.objects.filter(id=self.catalog1.id, availability=False).exists())


class DeleteFileFormTestCase(TestCase):

    def setUp(self):
        self.client = Client()
        self.user = User.objects.create_user(username='testuser', password='12345')
        self.file1 = File.objects.create(name="file1", source_code="print('Hello, World!')", owner=self.user)
        self.file2 = File.objects.create(name="file2", source_code="print('Goodbye, World!')", owner=self.user)

    def test_delete_file_form_submission(self):
        form_data = {'file': self.file1.id}
        response = self.client.post(reverse('deletefile'), data=form_data)
        self.assertEqual(response.status_code, 302)
        self.assertFalse(File.objects.filter(name='file1', availability=False).exists())
        self.assertTrue(File.objects.filter(name='file2', availability=True).exists())

    def test_delete_file_form_submission_invalid_file(self):
        form_data = {'file': 999}
        response = self.client.post(reverse('deletefile'), data=form_data)
        self.assertEqual(response.status_code, 302)
        self.assertTrue(File.objects.filter(name='file1', availability=True).exists())
        self.assertTrue(File.objects.filter(name='file2', availability=True).exists())


class TestRegistrationForm(TestCase):

    def setUp(self):
        self.client = Client()

    def test_submit_valid_form(self):
        data = {
            'username': 'testuser',
            'login': 'testlogin',
            'password': 'testpassword'
        }
        response = self.client.post(reverse('register'), data)
        self.assertEqual(response.status_code, 302)
        self.assertEqual(response.url, reverse('index'))

        user = User.objects.get(username='testuser')
        self.assertIsNotNone(user)
        self.assertEqual(user.username, 'testuser')
        self.assertEqual(user.email, 'testlogin')


class TestLoginForm(TestCase):

    def setUp(self):
        self.client = Client()
        self.user = User.objects.create_user(username='testuser', password='testpassword')

    def test_submit_valid_form(self):
        data = {
            'username': 'testuser',
            'password': 'testpassword'
        }
        response = self.client.post(reverse('loginview'), data)
        self.assertEqual(response.status_code, 302)
        self.assertEqual(response.url, reverse('index'))

    def test_invalid_login(self):
        data = {
            'username': 'invaliduser',
            'password': 'testpassword'
        }
        response = self.client.post(reverse('loginview'), data)
        self.assertEqual(response.status_code, 200)

    def test_invalid_password(self):
        data = {
            'username': 'testuser',
            'password': 'invalidpassword'
        }
        response = self.client.post(reverse('loginview'), data)
        self.assertEqual(response.status_code, 200)

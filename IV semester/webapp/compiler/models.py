from django.db import models
from django.utils import timezone
from django.core.validators import MinValueValidator
from django.contrib.auth.models import User


class CompilerUser(models.Model):
    username = models.CharField(max_length=255)
    login = models.CharField(max_length=255)
    password = models.CharField(max_length=255)

    def __str__(self):
        return self.username


class Catalog(models.Model):
    name = models.CharField(max_length=255)
    description = models.TextField(blank=True, null=True)
    creation_date = models.DateTimeField(default=timezone.now)
    owner = models.ForeignKey(User, on_delete=models.CASCADE, blank=True, null=True)
    availability = models.BooleanField(default=True)
    availability_change_date = models.DateTimeField(default=timezone.now)
    last_changed = models.DateTimeField(default=timezone.now)
    parent_catalog = models.ForeignKey('self', on_delete=models.CASCADE,
                                       blank=True, null=True, related_name='subcatalogs')

    def __str__(self):
        return self.name

    def recursetree(self):
        result = ''
        files = File.objects.filter(catalog=self)
        if self.availability:
            result += f'<li><details><summary>{self.name}</summary><ul>'
            if self.subcatalogs.all().exists():
                for subcatalog in self.subcatalogs.all():
                    if subcatalog.availability:
                        result += subcatalog.recursetree()
            for file in files:
                if file.availability:
                    result += f'<li><button class="choose-file-btn" data-id="{file.id}">{file.name}</button></li>'
            result += '</ul></details></li>'
            return result


class File(models.Model):
    catalog = models.ForeignKey(Catalog, on_delete=models.CASCADE, blank=True, null=True)
    source_code = models.TextField(blank=True, null=True)
    name = models.CharField(max_length=255)
    description = models.TextField(blank=True, null=True)
    creation_date = models.DateTimeField(default=timezone.now)
    owner = models.ForeignKey(User, on_delete=models.CASCADE, blank=True, null=True)
    availability = models.BooleanField(default=True)
    availability_change_date = models.DateTimeField(default=timezone.now)
    last_changed = models.DateTimeField(default=timezone.now)

    def __str__(self):
        return self.name


class SectionType(models.Model):
    CATEGORIES = (
        ('PROCEDURA', 'Procedura'),
        ('KOMENTARZ', 'Komentarz'),
        ('DYREKTYWA_KOMPILATORA', 'Dyrektywa kompilatora'),
        ('DEKLARACJE_ZMIENNYCH', 'Deklaracje zmiennych'),
        ('WSTAWKA_ASM', 'Wstawka asemblerowa'),
    )
    category = models.CharField(max_length=255, choices=CATEGORIES)

    def __str__(self):
        return self.category


class SectionStatus(models.Model):
    STATUSES = (
        ('KOMPILUJE_SIE_Z_OSTRZEZENIAMI', 'Kompiluje się z ostrzeżeniami'),
        ('KOMPILUJE_SIE_BEZ_OSTRZEZEN', 'Kompiluje się bez ostrzeżeń'),
        ('NIE_KOMPILUJE_SIE', 'Nie kompiluje się'),
    )
    status = models.CharField(max_length=255, choices=STATUSES)

    def __str__(self):
        return self.status


class StatusData(models.Model):
    status = models.CharField(max_length=255)
    line_of_error = models.PositiveIntegerField(validators=[MinValueValidator(0)])

    def __str__(self):
        return self.status + ':' + str(self.line_of_error)


class Section(models.Model):
    file = models.ForeignKey(File, on_delete=models.CASCADE)
    parent_section = models.ForeignKey('self', on_delete=models.CASCADE,
                                       blank=True, null=True, related_name='subsections')
    name = models.CharField(blank=True, null=True, max_length=255)
    description = models.TextField(blank=True, null=True)
    creation_date = models.DateTimeField(default=timezone.now)
    section_start = models.PositiveIntegerField(validators=[MinValueValidator(0)])
    section_end = models.PositiveIntegerField(validators=[MinValueValidator(0)])
    type_of_section = models.ForeignKey(SectionType, on_delete=models.CASCADE)
    section_status = models.ForeignKey(SectionStatus, on_delete=models.CASCADE)
    status_data = models.ForeignKey(StatusData, on_delete=models.CASCADE)
    contents = models.TextField(blank=True, null=True)

    def __str__(self):
        return self.name

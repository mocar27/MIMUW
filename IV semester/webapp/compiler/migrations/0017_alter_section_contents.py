# Generated by Django 4.1.7 on 2023-05-08 12:45

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('compiler', '0016_alter_section_section_end_and_more'),
    ]

    operations = [
        migrations.AlterField(
            model_name='section',
            name='contents',
            field=models.TextField(blank=True, null=True),
        ),
    ]

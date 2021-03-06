"""web URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/1.8/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  url(r'^$', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  url(r'^$', Home.as_view(), name='home')
Including another URLconf
    1. Add a URL to urlpatterns:  url(r'^blog/', include('blog.urls'))
"""
from django.conf.urls import include, url
from django.contrib import admin

urlpatterns = [
    url(r'^general/', include('general.urls', namespace='general')),
    url(r'^apache/', include('apache.urls', namespace='apache')),
    url(r'^bash/', include('bash.urls', namespace='bash')),
    url(r'^admin/', include(admin.site.urls)),

    # index
    url(r'^$', 'general.views.status', name='index'),

    url(r'^user/login/$', 'web.views.user_login'),
    url(r'^user/auth$', 'web.views.user_auth'),
    url(r'^user/logout/$', 'web.views.user_logout'),
    url(r'^user/invalid_login/$', 'web.views.user_invalid_login'),
]

admin.site.site_header = 'SLAS web module administration tool'

 --drop DATABASE t01_library;
 --CREATE DATABASE t01_library;
 
 create type state_type as enum ('excellent', 'good', 'satisfactory', 'dilapidated', 'lost');
 create type where_type as enum ('in stock', 'issued', 'reversed');
 
 create table if not exists public.author (
 author_key serial primary key,
 surname varchar (30) not null, 
 name varchar (30) not null
 );
 
 
  create table if not exists public.reader (
 library_card serial primary key,
 surname varchar (30) not null, 
 name varchar (30) not null,
 date_born date not null,
 gender char (1) check (gender in ('m','f')) not null,
 registration_date date not null
 );
 
 create table if not exists public.publishing_house ( 
 house_key serial primary key, 
 title varchar(100) not null,
 city varchar (50) not null
 );
 
 create table if not exists public.book (
book_key serial primary key, 
title varchar(100) not null,
author_key INT not null, 
FOREIGN KEY (author_key) REFERENCES public.author(author_key),
city varchar (50) not null,
code_pub_h INT not null,
version VARCHAR(50) NOT NULL,
year INT NOT NULL,
circulation INT NOT NULL,
foreign key (code_pub_h) references public.publishing_house (house_key)
 );
 

 create table if not exists public.book_instance (
 internal_inventory_number serial primary key,
 book_information int not null,
 foreign key (book_information) references public.book(book_key),
 state state_type not null, 
 status where_type, 
location varchar (300) CHECK (location ~ '^/\d+/\d+/\d+$') 
 );
 
 
 create table if not exists public.issuance (
 date_issuance timestamp not NULL,
 expected_date_return date not NULL,
 actual_date_return date NULL,
 reader_ticket_id INT NOT NULL,
 book_instance_id INT NOT NULL,
  FOREIGN KEY (reader_ticket_id) REFERENCES public.reader(library_card),
  FOREIGN KEY (book_instance_id) REFERENCES public.book_instance(internal_inventory_number),
  primary key (reader_ticket_id, book_instance_id)
 );
 
 
 create table if not exists public.booking (
 number_reservation serial primary key,
 id_reader_number int,
 foreign key (id_reader_number) references public.reader (library_card),
 information_of_book int,
 foreign key (information_of_book) references public.book (book_key),
 min_condition state_type not null,
 date_reservation date not null,
 time_reservation time not NULL
 );
 

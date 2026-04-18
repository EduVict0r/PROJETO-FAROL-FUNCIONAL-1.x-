PRAGMA foreign_keys = ON;

CREATE TABLE administrador (
    id_admin INTEGER PRIMARY KEY AUTOINCREMENT,
    nome_admin TEXT NOT NULL CHECK(length(nome_admin) <= 90),
    email_admin TEXT NOT NULL UNIQUE CHECK(length(email_admin) <= 90),
    cpf_admin TEXT NOT NULL UNIQUE CHECK(length(cpf_admin) = 11),
    senha_admin TEXT NOT NULL CHECK(length(senha_admin) <= 255)
);

CREATE TABLE endereco (
    id_endereco INTEGER PRIMARY KEY AUTOINCREMENT,
    rua_endereco TEXT NOT NULL CHECK(length(rua_endereco) <= 100),
    numero_endereco INTEGER,
    bairro TEXT NOT NULL CHECK(length(bairro) <= 45),
    complemento_endereco TEXT CHECK(length(complemento_endereco) <= 100),
    cep TEXT NOT NULL UNIQUE CHECK(length(cep) = 8),
    cidade TEXT NOT NULL CHECK(length(cidade) <= 45)
);

CREATE TABLE usuario (
    id_usuario INTEGER PRIMARY KEY AUTOINCREMENT,
    nome_usuario TEXT NOT NULL CHECK(length(nome_usuario) <= 90),
    cpf_usuario TEXT NOT NULL UNIQUE CHECK(length(cpf_usuario) = 11),
    senha TEXT NOT NULL CHECK(length(senha) <= 255),
    nascimento TEXT NOT NULL
);

CREATE TABLE chamada_emergencia (
    id_chamada_emergencia INTEGER PRIMARY KEY AUTOINCREMENT,
    numero_emergencia INTEGER NOT NULL
);

CREATE TABLE tipo_ocorrencia (
    id_tipo_ocorrencia INTEGER PRIMARY KEY AUTOINCREMENT,
    instituicao TEXT NOT NULL CHECK(length(instituicao) <= 20)
);

CREATE TABLE registro (
    id_registro INTEGER PRIMARY KEY AUTOINCREMENT,
    data_hora_registro DATETIME DEFAULT CURRENT_TIMESTAMP,
    fk_usuario INTEGER,
    fk_chamada_emergencia INTEGER,
    fk_tipo_ocorrencia INTEGER,
    FOREIGN KEY (fk_usuario) REFERENCES usuario(id_usuario),
    FOREIGN KEY (fk_chamada_emergencia) REFERENCES chamada_emergencia(id_chamada_emergencia),
    FOREIGN KEY (fk_tipo_ocorrencia) REFERENCES tipo_ocorrencia(id_tipo_ocorrencia)
);

CREATE TABLE classificacao_ocorrencia (
    id_classificacao_ocorrencia INTEGER PRIMARY KEY AUTOINCREMENT,
    nome_classificacao TEXT NOT NULL CHECK(length(nome_classificacao) <= 45),
    fk_tipo_ocorrencia INTEGER,
    FOREIGN KEY (fk_tipo_ocorrencia) REFERENCES tipo_ocorrencia(id_tipo_ocorrencia)
);

CREATE TABLE ocorrencia (
    id_ocorrencia INTEGER PRIMARY KEY AUTOINCREMENT,
    descricao_ocorrencia TEXT NOT NULL CHECK(length(descricao_ocorrencia) <= 1000),
    latitude REAL NOT NULL,
    longitude REAL NOT NULL,
    data_hora TEXT NOT NULL,
    bairro_ocorrencia TEXT NOT NULL,
    fk_usuario INTEGER NOT NULL,
    fk_classificacao_ocorrencia INTEGER,
    FOREIGN KEY (fk_usuario) REFERENCES usuario(id_usuario),
    FOREIGN KEY (fk_classificacao_ocorrencia) REFERENCES classificacao_ocorrencia(id_classificacao_ocorrencia)
);

INSERT INTO chamada_emergencia (numero_emergencia) VALUES (190);
INSERT INTO chamada_emergencia (numero_emergencia) VALUES (192);
INSERT INTO chamada_emergencia (numero_emergencia) VALUES (193);

INSERT INTO tipo_ocorrencia (instituicao) VALUES ('Policial');
INSERT INTO tipo_ocorrencia (instituicao) VALUES ('Samu');
INSERT INTO tipo_ocorrencia (instituicao) VALUES ('Bombeiro');

INSERT INTO classificacao_ocorrencia (nome_classificacao, fk_tipo_ocorrencia) VALUES
('Homicidio', 1),
('Furto', 1),
('Estupro', 1),
('Feminicidio', 1),
('Assalto', 1),
('Latrocinio', 1),
('Acidentes de trânsito', 2),
('Incidente', 2),
('Convulsão', 2),
('Queda', 2),
('Mal Subito', 2),
('Afogamento', 2),
('Incêndio', 3),
('Resgate em Altura', 3),
('Desabamento', 3),
('Vazamento de Gás', 3),
('Resgate Aquático', 3);

INSERT INTO administrador (nome_admin, email_admin, cpf_admin, senha_admin)
VALUES ('Administrador', 'admin@farol.com', '12345678901', '1234');
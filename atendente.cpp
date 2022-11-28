#include "atendente.h"
namespace dwp{
Atendente::Atendente()
{
    Atendente::secretaria_geral = false;
    Atendente::usuario = new Sistema();
}

Atendente::Atendente(Sistema *usuario, bool geral, long *medicos)
{
    Atendente::medicos_assessorados = new long[3];
    Atendente::medicos_assessorados = medicos;
    Atendente::secretaria_geral = geral,
    //usuario -> mostar_usuario();
    Atendente::usuario = new Sistema{*usuario};
}

Atendente::Atendente(QString nome, long cpf, QString email, long telefone, long telefone_whatsapp, bool secretaria_geral, long *medicos)
{
    Atendente::secretaria_geral = secretaria_geral;
    Atendente::usuario = new Sistema(nome, cpf, email, telefone, telefone_whatsapp);
    Atendente::medicos_assessorados = new long[3];
    Atendente::medicos_assessorados = medicos;
}

void Atendente::cadastrar_usuario(int tipo, Sistema *usuario)
{
    //verificando o tipo de usuario a ser criado
    QDir path{};

    qDebug() << path.path();
    switch(tipo)
    {
    case 0:
        if(path.mkdir(QString::fromStdString("atendente")))
        {
            std::cout << "sucesso ao criar pasta atendente";
        }
        break;
    case 1:
        if(path.mkdir(QString::fromStdString("medico")))
        {
            std::cout << "sucesso ao criar pasta medico";
        }
        break;
    case 2:
        if(path.mkdir(QString::fromStdString("paciente")))
        {
            std::cout << "sucesso ao criar pasta pasciente";
        }
        break;
    default:
        std::cout << "erro ao tentar criar pasta de usuario" << endl;
        exit(1);
        break;
    }

    //apenas atendentes gerais podem criar novas atendentes
    if(tipo == 0 && !Atendente::secretaria_geral)
    {
        std::cout<< "Erro, este usuário não tem permissão para criar um usuário" << endl;
        exit(1);
    }
}

/**
 * Método que cadastra uma funcionaria em um arquivo, os dados cadastrados
 * são os parâmetros da funcionario
 *
 * @brief Atendente::cadastrar_atendente
 * @param atendente - atendente que deve ser cadastrada
 * @return true - caso o cadastro tenha executado com sucesso
 */
bool Atendente::cadastrar_atendente(Atendente *atendente)
{
    QDir path{};
    QString filename{"dados.txt"};
    QFile arquivo{filename};

    if(!get_Secretaria_geral())
    {
        std::cout << "erro, você não tem permissões para cadastrar uma secretaria" << endl;
        return false;
    }

    std::cout << "criando pasta secretaria" << endl;

    if(path.mkpath(QString::fromStdString("atendente/")+QString::number(atendente->usuario->getNum_matricula())))
    {
        std::cout << "Pasta da secretaria de id:" + std::to_string(atendente->getNum_matricula()) + " criada" << endl;
    }
    path.setCurrent(QString::fromStdString("./atendente/")+QString::number(atendente->usuario->getNum_matricula()));
    std::cout << path.path().toStdString();

    //por padrão vamos sobreescrever dados existentes
    if(arquivo.exists())
    {
        std::cout << "ATENÇÃO:: já existe um arquivo de dados no diretorio do usuário, precione qualquer tecla para sobreescrever o arquivo atual" << endl;
        getchar();
        arquivo.remove();
    }
    if (!arquivo.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        std::cout << "Erro ao tentar abrir o arquivo em modo de escrita" << endl;

        return false;
    }

    QTextStream in{&arquivo};

    in << atendente->usuario->getNome()                                      + "\n" +
          QString::number(atendente->usuario->getCpf())                      + "\n" +
          atendente->usuario->getEmail()                                     + "\n" +
          QString::number(atendente->usuario->getTelefone())                 + "\n" +
          QString::number(atendente->usuario->getTelefone_whatsapp())        + "\n" +
          QString::number(atendente->usuario->getNum_matricula())            + "\n" +
          QString::number(atendente->get_Secretaria_geral())          + "\n" +
          QString::number(atendente->get_medicos_assessorados()[0])   + "," +
          QString::number(atendente->get_medicos_assessorados()[1])   + "," +
          QString::number(atendente->get_medicos_assessorados()[2]);

    std::cout << "arquivo de dados da secretaria criada" << endl;
    arquivo.close();

    return true;
}


bool Atendente::adicionar_agenda(QString *agenda)
{
    QDir *path = Atendente::set_path();
    QString tmp{};
    QFile arquivo{"agenda.txt"};

    if(!arquivo.exists())
    {
        std::cout << "Atenção. Agenda da atendente não existe. Criando arquivo..." << endl;

    }
    if (!arquivo.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        std::cout << "Erro ao criar arquivo;" << endl;
        return false;
    }

    QTextStream in{&arquivo};

    while(!arquivo.atEnd())
    {
         in.seek(EOF);
    }

    in << + "[\n"
          + *agenda
          + "\n]";

    arquivo.close();
    return true;
}

/**
QString* formatar_agenda()
{
    QString *agenda = new QString();
    int dia;

    cin >>
}*/


/**
 * @brief Atendente::set_path
 * @return path do id da funcionaria.
 */
QDir *Atendente::set_path()
{
    QDir *path = new QDir();

    int id = Atendente::usuario->getNum_matricula();

    if(!path->absolutePath().contains("atendente/" + QString::number(id)) && !path->setCurrent(QString::fromStdString("./atendente/") + QString::number(id)))
    {
        std::cout << "Path atual: " << path->absolutePath().toStdString() << endl;
        std::cout << "Atenção. Pasta \"Atendentes\" inexistente ou atendente de id:" << id << "não existe." << endl;
        exit(1);
    }

    std::cout << path->path().toStdString();
    return path;
}

//TODO: temos que colocar as variaveis long em longlong já que long no c++ vai até 4294967295
Atendente* Atendente::get_dados_atendente(int id)
{
    QDir path{};
    QString filename{"dados.txt"};
    QFile arquivo{filename};
    Atendente *retornar = new Atendente();
    bool ok;
    QStringList lista_acessorados;
    long *cpfs = new long[3];


    if(!path.setCurrent(QString::fromStdString("./atendente/")+QString::number(id)))
    {
        std::cout << "Erro ao tentar abrir a pasta da funcionaria" << endl;
        if(!path.exists("atendente/"+QString::number(id))){
            std::cout << "Atenção. Pasta \"Atendentes\" inexistente ou atendente de id:" << id << "não existe." << endl;
        }
        exit(1);
    }

    if (!arquivo.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        std::cout << "Erro ao tentar abrir o arquivo em modo de leitura" << endl;
        exit(1);
        //return false;
    }
    QTextStream in{&arquivo};

    retornar->usuario->setNome(in.readLine());

    retornar->usuario->setCpf(in.readLine().toLong());
    retornar->usuario->setEmail(in.readLine());
    retornar->usuario->setTelefone(in.readLine().toLong());
    retornar->usuario->setTelefone_whatsapp(in.readLine().toLong());
    retornar->usuario->setNum_matricula(in.readLine().toInt(&ok, 10));
    retornar->set_Secretaria_geral(in.readLine().toInt(&ok, 2));

    lista_acessorados = in.readLine().split(',');
    cpfs[0] = lista_acessorados[0].toLong();
    cpfs[1] = lista_acessorados[1].toLong();
    cpfs[2] = lista_acessorados[2].toLong();
    retornar->set_Medicos_acessorados(cpfs);

    arquivo.close();
    return retornar;
}



//bool Atendente::exist()

bool Atendente::get_Secretaria_geral()
{
    return Atendente::secretaria_geral;
}

void Atendente::set_Secretaria_geral(bool resp)
{
    Atendente::secretaria_geral = resp;
}


long *Atendente::get_medicos_assessorados()
{
    return Atendente::medicos_assessorados;
}

void Atendente::set_Medicos_acessorados(long *resp)
{
    Atendente::medicos_assessorados = resp;
}

/**
 * @brief Atendente::mostrar_dados_atendente
 * mostra na tela todos os dados da atendente
 * inclusive os medicos assessorados
 */
void Atendente::mostrar_dados_atendente()
{
    std::cout << Atendente::usuario->getNome().toStdString() << endl;
    std::cout << std::to_string(Atendente::usuario->getCpf()) << endl;
    std::cout << Atendente::usuario->getEmail().toStdString() << endl;
    std::cout <<  std::to_string(Atendente::usuario->getTelefone()) << endl;
    std::cout <<  std::to_string(Atendente::usuario->getTelefone_whatsapp()) << endl;
    std::cout << std::to_string(Atendente::usuario->getNum_matricula()) << endl;
    std::cout << std::to_string(Atendente::get_Secretaria_geral()) << endl;
    std::cout << Atendente::get_medicos_assessorados()[0] << endl;
    std::cout << Atendente::get_medicos_assessorados()[1] << endl;
    std::cout << Atendente::get_medicos_assessorados()[2]<< endl;
}
}
